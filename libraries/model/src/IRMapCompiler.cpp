////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMapCompiler.cpp (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRMapCompiler.h"
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMetadata.h"
#include "IRModelProfiler.h"
#include "Model.h"
#include "ModelOptimizer.h"
#include "OptimizationPassRegistry.h"
#include "OutputNode.h"

// emitters
#include "EmitterException.h"
#include "LLVMUtilities.h"
#include "Variable.h"

// utils
#include "Logger.h"
#include "StringUtil.h"

// stl
#include <tuple>

namespace ell
{
namespace model
{
    namespace
    {
        bool IsConvolutionalLayerNode(const Node& node)
        {
            return (node.GetRuntimeTypeName().find("ConvolutionalLayerNode") == 0);
        }
    }

    using namespace logging;

    IRMapCompiler::IRMapCompiler()
        : IRMapCompiler(MapCompilerOptions{})
    {
    }

    IRMapCompiler::IRMapCompiler(const MapCompilerOptions& settings)
        : MapCompiler(settings), _moduleEmitter(settings.moduleName, settings.compilerSettings), _profiler(), _optimizer(settings)
    {
        Log() << "Initializing IR map compiler" << EOL;
        Log() << "Initializing optimizer" << EOL;
        OptimizationPassRegistry::AddPassesToOptimizer(_optimizer, settings.optimizerSettings);

        _nodeRegions.emplace_back();
    }

    // EnsureValidMap fixes up the model if necessary and checks that inputs/outputs are compilable
    void IRMapCompiler::EnsureValidMap(Map& map)
    {
        if (map.NumInputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single input");
        }

        if (map.NumOutputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single output");
        }

        Log() << "Ensuring map is valid..." << EOL;

        // if output isn't a simple port, add an output node to model
        // TODO: remove this in favor of normalizing pass
        auto out = map.GetOutput(0);
        MemoryShape shape{ static_cast<int>(out.Size()) }; // default shape from PortElementsBase::Size()
        auto outNodes = map.GetOutputNodes();
        if (!outNodes.empty())
        {
            shape = outNodes[0]->GetShape();
            Log() << "Output nodes present. Setting shape to first output node" << EOL;
        }
    }

    std::string IRMapCompiler::GetNamespacePrefix() const
    {
        return GetModule().GetModuleName();
    }

    std::string IRMapCompiler::GetGlobalName(const Node& node, const std::string& baseName) const
    {
        // e.g "ELL_GRUNodeReset_1541".
        return GetNamespacePrefix() + "_" + baseName + "_" + node.GetId().ToString();
    }

    std::string IRMapCompiler::GetPredictFunctionName() const
    {
        return GetMapCompilerOptions().mapFunctionName;
    }

    IRCompiledMap IRMapCompiler::Compile(Map map)
    {
        // phases of compilation / refinement / optimization
        //
        // Fix map outputs (EnsureValidMap)
        //
        // pre-refinement phase:
        // Refine 1 (refine NN predictor nodes)
        // Optimize 1 (set preferred conv type)
        //
        // refinement phase:
        // Refine 2
        //
        // post-refine phase:
        // Optimize 2
        //
        // Compile
        // IR optimization

        Log() << "Compile called for map" << EOL;

        EnsureValidMap(map);

        //
        // Temporary special-purpose code to allow the "SetConvolutionMethod" optimization pass to work.
        // When refinement is an integrated part of optimization, then this special-case code will disappear.
        //
        Log() << "Refining the model..." << EOL;
        model::TransformContext noRefineConvLayerNodesContext{ this, [this](const model::Node& node) { return IsConvolutionalLayerNode(node) || node.IsCompilable(this) ? model::NodeAction::compile : model::NodeAction::refine; } };
        map.Refine(noRefineConvLayerNodesContext);

        Log() << "Optimizing the model..." << EOL;
        map.Optimize(_optimizer);

        Log() << "Refining the model again..." << EOL;
        model::TransformContext refineContext{ this, [this](const model::Node& node) { return node.IsCompilable(this) ? model::NodeAction::compile : model::NodeAction::refine; } };
        map.Refine(refineContext);

        Log() << "Optimizing the model again..." << EOL;
        map.Optimize(_optimizer);

        // Renaming callbacks based on map compiler parameters
        // Note: a more elegant solution is emit variables which get assigned to
        // function pointers at runtime (prior to computing the map).
        Log() << "Renaming callbacks..." << EOL;
        map.RenameCallbacks(GetMapCompilerOptions().sourceFunctionName, GetMapCompilerOptions().sinkFunctionName);

        // Now the model ready for compiling
        if (GetMapCompilerOptions().profile)
        {
            Log() << "Enabling profiling in emitted IR" << EOL;
            GetModule().AddPreprocessorDefinition(GetNamespacePrefix() + "_PROFILING", "1");
        }
        _profiler = { GetModule(), map.GetModel(), GetMapCompilerOptions().profile };
        _profiler.EmitInitialization();

        // Now we have the refined map, compile it
        Log() << "Compiling map..." << EOL;
        CompileMap(map, GetPredictFunctionName());

        // Emit runtime model APIs
        EmitModelAPIFunctions(map);

        // Finish any profiling stuff we need to do and emit functions
        _profiler.EmitModelProfilerFunctions();

        auto module = std::make_unique<emitters::IRModuleEmitter>(std::move(_moduleEmitter));

        if (GetMapCompilerOptions().compilerSettings.optimize)
        {
            // Save callback declarations in case they get optimized away
            std::vector<std::tuple<std::string, llvm::FunctionType*, std::vector<std::string>>> savedCallbacks;
            auto callbacks = emitters::GetFunctionsWithTag(*module, emitters::c_callbackFunctionTagName);
            for (auto& callbackInfo : callbacks)
            {
                savedCallbacks.emplace_back(callbackInfo.function->getName(), callbackInfo.function->getFunctionType(), callbackInfo.values);
            }

            emitters::IROptimizer optimizer(*module);
            optimizer.AddStandardPasses();
            module->Optimize(optimizer);

            // Reinsert callback declarations after optimization
            for (const auto& savedCallback : savedCallbacks)
            {
                auto functionName = std::get<0>(savedCallback);
                module->DeclareFunction(functionName, std::get<1>(savedCallback));
                module->IncludeInCallbackInterface(functionName, std::get<2>(savedCallback)[0]);
            }
        }

        return IRCompiledMap(std::move(map), GetMapCompilerOptions().mapFunctionName, GetMapCompilerOptions(), std::move(module), GetMapCompilerOptions().verifyJittedModule);
    }

    void IRMapCompiler::EmitModelAPIFunctions(const Map& map)
    {
        EmitGetInputSizeFunction(map);
        EmitGetOutputSizeFunction(map);
        EmitGetSinkOutputSizeFunction(map);
        EmitGetNumNodesFunction(map);
        EmitShapeEnum();
        EmitGetInputShapeFunction(map);
        EmitGetOutputShapeFunction(map);
        EmitGetSinkOutputShapeFunction(map);
    }

    void IRMapCompiler::EmitGetInputSizeFunction(const Map& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        const emitters::NamedLLVMTypeList parameters = { { "index", int32Type } };
        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetInputSize", int32Type, parameters);
        function.IncludeInHeader();

        std::vector<int> sizes;
        for(size_t i = 0 , n = map.GetNumInputs(); i < n; ++i)
        {            
            sizes.push_back(map.GetInputSize(i));
        }
        EmitSizeConditionals(function, sizes);
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetOutputSizeFunction(const Map& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        const emitters::NamedLLVMTypeList parameters = { { "index", int32Type } };
        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetOutputSize", int32Type, parameters);
        function.IncludeInHeader();

        std::vector<int> sizes;
        for (size_t i = 0, n = map.GetNumOutputs(); i < n; ++i)
        {
            sizes.push_back(map.GetOutputSize(i));
        }
        EmitSizeConditionals(function, sizes);
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetSinkOutputSizeFunction(const Map& map)
    {
        auto nodes = map.GetSinkNodes();
        if (nodes.size() > 0)
        {
            auto& context = _moduleEmitter.GetLLVMContext();
            auto int32Type = llvm::Type::getInt32Ty(context);

            const emitters::NamedLLVMTypeList parameters = { { "index", int32Type } };
            auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetSinkOutputSize", int32Type, parameters);
            function.IncludeInHeader();

            std::vector<int> sizes;
            for (auto ptr = nodes.begin(), end = nodes.end(); ptr != end; ptr++)
            {
                const ell::model::Node* node = *ptr;
                sizes.push_back(node->GetOutputPort(0)->GetMemoryLayout().GetActiveSize().NumElements());
            }
            EmitSizeConditionals(function, sizes);
            _moduleEmitter.EndFunction();
        }
    }

    void IRMapCompiler::EmitSizeConditionals(emitters::IRFunctionEmitter& fn, std::vector<int> sizes)
    {
        // This is the type of code we are trying to generate for the GetInputSize and GetOutputSize functions:
        //
        // int model_GetInputSize(int index)
        // {
        //     if (index == 0) {
        //         return 224;
        //     }
        //     if (index == 1) {
        //         return 10;
        //     }
        //     return 0;
        // }
        //
        auto arguments = fn.Arguments().begin();
        auto indexArgument = &(*arguments++);
        auto pMainBlock = fn.GetCurrentBlock();

        std::vector<llvm::BasicBlock*> blocks;
        blocks.push_back(pMainBlock);
        // create the final block in the case the index is out of range or there are no shapes and reutrn empty TensorShape.
        auto pDoneBlock = fn.BeginBlock("NoMatchBlock");
        {
            fn.Return(fn.Literal(0));
        }

        if (!sizes.empty())
        {
            int index = 0;
            for (auto ptr = sizes.begin(), end = sizes.end(); ptr != end; ptr++, index++)
            {
                int size = *ptr;
                std::string labelSuffix = std::to_string(index);

                auto followBlock = fn.BeginBlock("FollowBlock" + labelSuffix);
                auto thenBlock = fn.BeginBlock("ThenBlock" + labelSuffix);
                {
                    fn.Return(fn.Literal(size));
                }
                auto elseBlock = fn.BeginBlock("ElseBlock" + labelSuffix);
                {
                    fn.Branch(followBlock);
                }
                auto conditionBlock = fn.BeginBlock("IfBlock" + labelSuffix);
                {
                    auto compare = new llvm::ICmpInst(*conditionBlock, llvm::ICmpInst::ICMP_EQ, indexArgument, fn.Literal(index));
                    llvm::BranchInst::Create(thenBlock, elseBlock, compare, conditionBlock);
                }
                blocks.push_back(conditionBlock);
                blocks.push_back(followBlock);
            }
            fn.SetCurrentBlock(blocks[blocks.size() - 1]);
            fn.Branch(pDoneBlock);
        }

        fn.SetCurrentBlock(pMainBlock);
        if (blocks.size() > 1)
        {
            fn.Branch(blocks[1]); // jump to first statement.
        }

        blocks.push_back(pDoneBlock);
        // ensure all blocks are properly chained with branch instructions and inserted into the function BasicBlockList.
        fn.ConcatenateBlocks(blocks);
    }

    const char* TensorShapeName = "TensorShape";

    void IRMapCompiler::EmitShapeEnum()
    {
        auto shapeType = _moduleEmitter.GetStruct(TensorShapeName);
        if (shapeType == nullptr)
        {
            auto int32Type = ell::emitters::VariableType::Int32;
            emitters::NamedVariableTypeList namedFields = { { "rows", int32Type }, { "columns", int32Type }, { "channels", int32Type } };
            auto shapeType = _moduleEmitter.GetOrCreateStruct(TensorShapeName, namedFields);
            _moduleEmitter.IncludeTypeInHeader(shapeType->getName());
        }
    }

    void IRMapCompiler::EmitShapeConditionals(emitters::IRFunctionEmitter& fn, std::vector<MemoryShape> shapes)
    {
        auto shapeType = _moduleEmitter.GetStruct(TensorShapeName);
        auto arguments = fn.Arguments().begin();
        auto indexArgument = &(*arguments++);
        auto shapeArgument = &(*arguments++);
        auto& emitter = _moduleEmitter.GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();
        auto rowsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeArgument, { fn.Literal(0), fn.Literal(0) });
        rowsPtr->setName("rows");
        auto columnsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeArgument, { fn.Literal(0), fn.Literal(1) });
        columnsPtr->setName("columns");
        auto channelsPtr = irBuilder.CreateInBoundsGEP(shapeType, shapeArgument, { fn.Literal(0), fn.Literal(2) });
        channelsPtr->setName("channels");
        auto pMainBlock = fn.GetCurrentBlock();

        std::vector<llvm::BasicBlock*> blocks;
        blocks.push_back(pMainBlock);
        // create the final block in the case the index is out of range or there are no shapes and reutrn empty TensorShape.
        auto pDoneBlock = fn.BeginBlock("NoMatchBlock");
        {
            fn.StoreZero(rowsPtr);
            fn.StoreZero(columnsPtr);
            fn.StoreZero(channelsPtr);
            fn.Return();
        }

        if (shapes.size() > 0)
        {
            int index = 0;
            for (auto ptr = shapes.begin(), end = shapes.end(); ptr != end; ptr++, index++)
            {
                std::string labelSuffix = std::to_string(index);

                auto followBlock = fn.BeginBlock("FollowBlock" + labelSuffix);
                auto thenBlock = fn.BeginBlock("ThenBlock" + labelSuffix);
                {
                    MemoryShape shape = *ptr;
                    shape.Resize(3);

                    fn.Store(rowsPtr, fn.Literal<int>(shape[0]));
                    fn.Store(columnsPtr, fn.Literal<int>(shape[1]));
                    fn.Store(channelsPtr, fn.Literal<int>(shape[2]));
                    fn.Return();
                }
                auto elseBlock = fn.BeginBlock("ElseBlock" + labelSuffix);
                {
                    fn.Branch(followBlock);
                }
                auto conditionBlock = fn.BeginBlock("IfBlock" + labelSuffix);
                {
                    auto compare = new llvm::ICmpInst(*conditionBlock, llvm::ICmpInst::ICMP_EQ, indexArgument, fn.Literal(index));
                    llvm::BranchInst::Create(thenBlock, elseBlock, compare, conditionBlock);
                }
                blocks.push_back(conditionBlock);
                blocks.push_back(followBlock);
            }
            fn.SetCurrentBlock(blocks[blocks.size() - 1]);
            fn.Branch(pDoneBlock);
        }

        fn.SetCurrentBlock(pMainBlock);
        if (blocks.size() > 1)
        {
            fn.Branch(blocks[1]); // jump to first statement.
        }

        blocks.push_back(pDoneBlock);
        // ensure all blocks are properly chained with branch instructions and inserted into the function BasicBlockList.
        fn.ConcatenateBlocks(blocks);
    }

    // This is the type of code we are trying to generate for the GetInputShape and GetOutputShape functions:
    //
    // void foo_GetInputShape(int index, struct TensorShape* s)
    // {
    //     if (index == 0) {
    //         s->rows = 224;
    //         s->columns = 224;
    //         s->channels = 3;
    //         return;
    //     }
    //     if (index == 1) {
    //         s->rows = 10;
    //         s->columns = 10;
    //         s->channels = 20;
    //         return;
    //     }
    //     s->rows = 0;
    //     s->columns = 0;
    //     s->channels = 0;
    // }
    //

    void IRMapCompiler::EmitGetInputShapeFunction(const Map& map)
    {
        // We have to create this interface because LLVM cannot reliably return structures on the stack.
        // void darknet_GetInputShape(int index, struct TensorShape* shape)
        auto shapeType = _moduleEmitter.GetStruct(TensorShapeName);
        auto& context = _moduleEmitter.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto int32Type = llvm::Type::getInt32Ty(context);

        const emitters::NamedLLVMTypeList parameters = { { "index", int32Type }, { "shape", shapeType->getPointerTo() } };
        const std::string functionName = GetNamespacePrefix() + "_GetInputShape";

        auto fn = _moduleEmitter.BeginFunction(functionName, voidType, parameters);
        fn.IncludeInHeader();

        std::vector<MemoryShape> shapes;
        for (size_t i = 0, n = map.GetNumInputs(); i < n; ++i)
        {
            shapes.push_back(map.GetInputShape(i));
        }

        EmitShapeConditionals(fn, shapes);
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetOutputShapeFunction(const Map& map)
    {
        // We have to create this interface because LLVM cannot reliably return structures on the stack.
        // void darknet_GetOutputShape(int index, struct TensorShape* shape)
        auto shapeType = _moduleEmitter.GetStruct(TensorShapeName);
        auto& context = _moduleEmitter.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto int32Type = llvm::Type::getInt32Ty(context);

        const emitters::NamedLLVMTypeList parameters = { { "index", int32Type }, { "shape", shapeType->getPointerTo() } };
        const std::string functionName = GetNamespacePrefix() + "_GetOutputShape";

        auto fn = _moduleEmitter.BeginFunction(functionName, voidType, parameters);
        fn.IncludeInHeader();

        std::vector<MemoryShape> shapes;
        for (size_t i = 0, n = map.GetNumOutputs(); i < n; ++i)
        {
            shapes.push_back(map.GetOutputShape(i));
        }

        EmitShapeConditionals(fn, shapes);
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetSinkOutputShapeFunction(const Map& map)
    {
        auto nodes = map.GetSinkNodes();
        if (nodes.size() > 0)
        {
            // We have to create this interface because LLVM cannot reliably return structures on the stack.
            // void darknet_GetOutputShape(int index, struct TensorShape* shape)
            auto shapeType = _moduleEmitter.GetStruct(TensorShapeName);
            auto& context = _moduleEmitter.GetLLVMContext();
            auto voidType = llvm::Type::getVoidTy(context);
            auto int32Type = llvm::Type::getInt32Ty(context);

            const emitters::NamedLLVMTypeList parameters = { { "index", int32Type }, { "shape", shapeType->getPointerTo() } };
            const std::string functionName = GetNamespacePrefix() + "_GetSinkOutputShape";

            auto fn = _moduleEmitter.BeginFunction(functionName, voidType, parameters);
            fn.IncludeInHeader();

            std::vector<MemoryShape> shapes;
            for (auto ptr = nodes.begin(), end = nodes.end(); ptr != end; ptr++)
            {
                const ell::model::Node* node = *ptr;
                shapes.push_back(node->GetOutputPort(0)->GetMemoryLayout().GetActiveSize());
            }
            EmitShapeConditionals(fn, shapes);
            _moduleEmitter.EndFunction();
        }
    }

    void IRMapCompiler::EmitGetNumNodesFunction(const Map& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        int numNodes = map.GetModel().Size();

        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetNumNodes", int32Type);
        function.IncludeInHeader();
        function.Return(function.Literal(numNodes));
        _moduleEmitter.EndFunction();
    }

    //
    // Node implementor methods:
    //

    emitters::LLVMValue IRMapCompiler::EnsurePortEmitted(const InputPortBase& port)
    {
        emitters::Variable* pVar = GetVariableForPort(port.GetReferencedPort());
        if (pVar == nullptr)
        {
            const Node* node = port.GetNode();
            throw emitters::EmitterException(emitters::EmitterError::unexpected, 
                utilities::FormatString("Error: missing port variable for '%s' port on node %s(%s)", port.GetName().c_str(), 
                    node->GetRuntimeTypeName().c_str(),
                    node->GetId().ToString().c_str()));
        }
        return GetModule().EnsureEmitted(*pVar);
    }

    emitters::LLVMValue IRMapCompiler::EnsurePortEmitted(const OutputPortBase& port)
    {
        auto pVar = GetOrAllocatePortVariable(port);
        return GetModule().EnsureEmitted(*pVar);
    }

    void IRMapCompiler::OnBeginCompileModel(const Model& model)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        if (currentFunction.GetCurrentRegion() == nullptr) // TODO: put this check in GetCurrentFunction()
        {
            currentFunction.AddRegion(currentFunction.GetCurrentBlock());
            Log() << "Creating a new region for " << currentFunction.GetFunctionName() << EOL;
        }

        // Tag the model function for declaration in the generated headers
        currentFunction.IncludeInHeader();
        currentFunction.IncludeInPredictInterface();

        _profiler.StartModel(currentFunction);
    }

    void IRMapCompiler::OnEndCompileModel(const Model& model)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        _profiler.EndModel(currentFunction);
    }

    void IRMapCompiler::OnBeginCompileNode(const Node& node)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        if (currentFunction.GetCurrentRegion() == nullptr)
        {
            Log() << "Creating new region for node in " << currentFunction.GetFunctionName() << EOL;
            currentFunction.AddRegion(currentFunction.GetCurrentBlock());
        }

        _profiler.InitNode(currentFunction, node);
        _profiler.StartNode(currentFunction, node);
    }

    void IRMapCompiler::OnEndCompileNode(const Node& node)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        assert(currentFunction.GetCurrentRegion() != nullptr);

        _profiler.EndNode(currentFunction, node);

        auto pCurBlock = currentFunction.GetCurrentBlock();
        if (pCurBlock != currentFunction.GetCurrentRegion()->End())
        {
            Log() << "Current node block is not end of function " << currentFunction.GetFunctionName()
                  << ". Setting it so it is." << EOL;
            currentFunction.GetCurrentRegion()->SetEnd(pCurBlock);
        }

        Log() << "Finished compiling node " << DiagnosticString(node) << EOL;
    }

    void IRMapCompiler::PushScope()
    {
        MapCompiler::PushScope();
        _nodeRegions.emplace_back();
    }

    void IRMapCompiler::PopScope()
    {
        MapCompiler::PopScope();
        assert(_nodeRegions.size() > 0);
        _nodeRegions.pop_back();
    }

    NodeMap<emitters::IRBlockRegion*>& IRMapCompiler::GetCurrentNodeBlocks()
    {
        assert(_nodeRegions.size() > 0);
        return _nodeRegions.back();
    }

    const Node* IRMapCompiler::GetUniqueParent(const Node& node)
    {
        Log() << "Trying to find unique parent of node " << DiagnosticString(node) << EOL;
        auto inputs = node.GetInputPorts();

        Log() << "Node has " << inputs.size() << " input ports" << EOL;

        const Node* pParentNode = nullptr;
        emitters::IRBlockRegion* pParentRegion = nullptr;
        for (auto input : inputs)
        {
            for (auto parentNode : input->GetParentNodes())
            {
                if (!HasSingleDescendant(*parentNode))
                {
                    return nullptr;
                }
                emitters::IRBlockRegion* pNodeRegion = GetCurrentNodeBlocks().Get(*parentNode);
                if (pNodeRegion != nullptr)
                {
                    Log() << "Got block region for parent node " << DiagnosticString(*parentNode) << EOL;
                    if (pParentRegion != nullptr && pNodeRegion != pParentRegion)
                    {
                        return nullptr;
                    }
                    pParentRegion = pNodeRegion;
                    pParentNode = parentNode;
                }
                else
                {
                    Log() << "Could not get block region for parent node " << DiagnosticString(*parentNode) << EOL;
                }
            }
        }
        return pParentNode;
    }

    void IRMapCompiler::NewNodeRegion(const Node& node)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        auto pBlock = currentFunction.Block(IdString(node));
        assert(pBlock != nullptr && "Got null new block");
        currentFunction.SetCurrentBlock(pBlock);
        auto currentRegion = currentFunction.AddRegion(pBlock);

        Log() << "Created region for node: " << DiagnosticString(node) << EOL;

        GetCurrentNodeBlocks().Set(node, currentRegion);

        if (GetMapCompilerOptions().compilerSettings.includeDiagnosticInfo)
        {
            currentFunction.Print(DiagnosticString(node) + '\n');
        }
    }

    bool IRMapCompiler::TryMergeNodeRegion(const Node& node)
    {
        Log() << "Trying to merge code regions for " << DiagnosticString(node) << EOL;

        auto pRegion = GetCurrentNodeBlocks().Get(node);
        if (pRegion == nullptr)
        {
            Log() << "Could not get code region for " << DiagnosticString(node) << EOL;
            return false;
        }

        const Node* pParentNode = GetUniqueParent(node);
        if (pParentNode == nullptr)
        {
            Log() << "Could not find a unique parent for " << DiagnosticString(node) << EOL;
            return false;
        }

        return TryMergeNodeRegions(*pParentNode, node);
    }

    bool IRMapCompiler::TryMergeNodeRegions(const Node& dest, const Node& src)
    {
        Log() << "Trying to merge parent node " << DiagnosticString(dest) << " with child node " << DiagnosticString(src) << EOL;

        emitters::IRBlockRegion* pDestRegion = GetCurrentNodeBlocks().Get(dest);
        if (pDestRegion == nullptr)
        {
            Log() << "Could not get code block region for parent node " << DiagnosticString(dest) << EOL;
            return false;
        }
        return TryMergeNodeIntoRegion(pDestRegion, src);
    }

    bool IRMapCompiler::TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestRegion, const Node& src)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();

        Log() << "Trying to merge emitted code for node " << DiagnosticString(src) << " with existing code region in " << currentFunction.GetFunctionName() << EOL;

        emitters::IRBlockRegion* pSrcRegion = GetCurrentNodeBlocks().Get(src);
        if (pSrcRegion == nullptr || pSrcRegion == pDestRegion)
        {
            Log() << "Code region for node " << (pSrcRegion ? " is same as region to merge" : " null") << EOL;
            return false;
        }

        Log() << "Setting end of current region to current block" << EOL;
        GetModule().GetCurrentRegion()->SetEnd(currentFunction.GetCurrentBlock());
        currentFunction.ConcatRegions(pDestRegion, pSrcRegion);
        GetCurrentNodeBlocks().Set(src, pDestRegion);
        return true;
    }

    emitters::IRBlockRegion* IRMapCompiler::GetMergeableNodeRegion(const PortElementBase& element)
    {
        const Node* pNode = nullptr;
        if (HasSingleDescendant(element))
        {
            emitters::Variable* pVar = GetVariableForPort(*element.ReferencedPort());
            if (pVar != nullptr && !pVar->IsLiteral())
            {
                pNode = element.ReferencedPort()->GetNode();
            }
        }

        return (pNode != nullptr) ? GetCurrentNodeBlocks().Get(*pNode) : nullptr;
    }

    llvm::LLVMContext& IRMapCompiler::GetLLVMContext()
    {
        return _moduleEmitter.GetLLVMContext();
    }

    //
    // Port variables
    //
    emitters::LLVMValue IRMapCompiler::LoadPortElementVariable(const PortElementBase& element)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        emitters::Variable* pVar = GetVariableForPort(*element.ReferencedPort());
        if (pVar->IsScalar())
        {
            throw emitters::EmitterException(emitters::EmitterError::vectorVariableExpected, "Error: got a scalar port variable");
        }
        else if (element.GetIndex() >= pVar->Dimension())
        {
            throw emitters::EmitterException(emitters::EmitterError::indexOutOfRange);
        }

        emitters::LLVMValue pVal = GetModule().EnsureEmitted(*pVar);
        auto valType = pVal->getType();
        bool needsDereference = valType->isPointerTy(); // TODO: Maybe this should be `isPtrOrPtrVectorTy()` or even `isPtrOrPtrVectorTy() || isArrayTy()`
        if (needsDereference)
        {
            return currentFunction.ValueAt(pVal, currentFunction.Literal((int)element.GetIndex()));
        }
        else
        {
            return pVal;
        }
    }
}
}
