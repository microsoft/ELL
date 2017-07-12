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
#include "IRModelProfiler.h"
#include "OutputNode.h"

// emitters
#include "EmitterException.h"
#include "Variable.h"

namespace ell
{
namespace model
{
    IRMapCompiler::IRMapCompiler()
        : IRMapCompiler(MapCompilerParameters{})
    {
    }

    IRMapCompiler::IRMapCompiler(const MapCompilerParameters& settings)
        : MapCompiler(settings), _moduleEmitter(settings.moduleName), _profiler()
    {
        _moduleEmitter.SetCompilerParameters(settings.compilerSettings);
        _nodeRegions.emplace_back();
    }

    void IRMapCompiler::EnsureValidMap(DynamicMap& map)
    {
        if (map.NumInputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single input");
        }

        if (map.NumOutputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single output");
        }

        // if output isn't a simple port, add an output node to model
        auto out = map.GetOutput(0);
        if (!out.IsFullPortOutput())
        {
            model::OutputNodeBase* outputNode = nullptr;
            switch (out.GetPortType())
            {
                case model::Port::PortType::boolean:
                    outputNode = map.GetModel().AddNode<model::OutputNode<bool>>(model::PortElements<bool>(out));
                    break;
                case model::Port::PortType::integer:
                    outputNode = map.GetModel().AddNode<model::OutputNode<int>>(model::PortElements<int>(out));
                    break;
                case model::Port::PortType::bigInt:
                    outputNode = map.GetModel().AddNode<model::OutputNode<int64_t>>(model::PortElements<int64_t>(out));
                    break;
                case model::Port::PortType::smallReal:
                    outputNode = map.GetModel().AddNode<model::OutputNode<float>>(model::PortElements<float>(out));
                    break;
                case model::Port::PortType::real:
                    outputNode = map.GetModel().AddNode<model::OutputNode<double>>(model::PortElements<double>(out));
                    break;
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }

            map.ResetOutput(0, outputNode->GetOutputPort());
        }
    }

    std::string IRMapCompiler::GetNamespacePrefix() const
    {
        return GetModule().GetModuleName();
    }

    std::string IRMapCompiler::GetPredictFunctionName() const
    {
        return GetMapCompilerParameters().mapFunctionName;
    }

    IRCompiledMap IRMapCompiler::Compile(DynamicMap map)
    {
        EnsureValidMap(map);
        model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() ? model::NodeAction::compile : model::NodeAction::refine; } };
        map.Refine(context);

        // Now the model ready for compiling
        if (GetMapCompilerParameters().profile)
        {
            GetModule().AddPreprocessorDefinition(GetNamespacePrefix() + "_PROFILING", "1");
        }
        _profiler = { GetModule(), map.GetModel(), GetMapCompilerParameters().profile };
        _profiler.EmitInitialization();

        // Now we have the refined map, compile it
        CompileMap(map, GetPredictFunctionName());

        // Emit runtime model APIs
        EmitModelAPIFunctions(map);

        // Finish any profiling stuff we need to do and emit functions
        _profiler.EmitModelProfilerFunctions();

        auto module = std::make_unique<emitters::IRModuleEmitter>(std::move(_moduleEmitter));
        module->SetTargetTriple(GetCompilerParameters().targetDevice.triple);
        module->SetTargetDataLayout(GetCompilerParameters().targetDevice.dataLayout);
        return IRCompiledMap(std::move(map), GetMapCompilerParameters().mapFunctionName, std::move(module));
    }

    void IRMapCompiler::EmitModelAPIFunctions(const DynamicMap& map)
    {
        EmitGetInputSizeFunction(map);
        EmitGetOutputSizeFunction(map);
        EmitGetNumNodesFunction(map);
    }

    void IRMapCompiler::EmitGetInputSizeFunction(const DynamicMap& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetInputSize", int32Type, {});
        function.IncludeInHeader();
        function.Return(function.Literal(static_cast<int>(map.GetInputSize())));
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetOutputSizeFunction(const DynamicMap& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetOutputSize", int32Type, {});
        function.IncludeInHeader();
        function.Return(function.Literal(static_cast<int>(map.GetOutputSize())));
        _moduleEmitter.EndFunction();
    }

    void IRMapCompiler::EmitGetNumNodesFunction(const DynamicMap& map)
    {
        auto& context = _moduleEmitter.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        int numNodes = map.GetModel().Size();

        auto function = _moduleEmitter.BeginFunction(GetNamespacePrefix() + "_GetNumNodes", int32Type, {});
        function.IncludeInHeader();
        function.Return(function.Literal(numNodes));
        _moduleEmitter.EndFunction();
    }

    //
    // Node implementor methods:
    //

    llvm::Value* IRMapCompiler::EnsurePortEmitted(const InputPortBase& port)
    {
        auto portElement = port.GetInputElement(0);
        return EnsurePortElementEmitted(portElement);
    }

    llvm::Value* IRMapCompiler::EnsurePortEmitted(const OutputPortBase& port)
    {
        auto pVar = GetOrAllocatePortVariable(port);
        return GetModule().EnsureEmitted(*pVar);
    }

    llvm::Value* IRMapCompiler::EnsurePortElementEmitted(const PortElementBase& element)
    {
        auto pVar = GetVariableForElement(element);
        if (pVar == nullptr)
        {
            throw emitters::EmitterException(emitters::EmitterError::notSupported, "Variable for output port not found");
        }
        return GetModule().EnsureEmitted(*pVar);
    }

    void IRMapCompiler::OnBeginCompileModel(const Model& model)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();
        if (currentFunction.GetCurrentRegion() == nullptr) // TODO: put this check in GetCurrentFunction()
        {
            currentFunction.AddRegion(currentFunction.GetCurrentBlock());
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
            currentFunction.GetCurrentRegion()->SetEnd(pCurBlock);
        }
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
        auto inputs = node.GetInputPorts();
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
                    if (pParentRegion != nullptr && pNodeRegion != pParentRegion)
                    {
                        return nullptr;
                    }
                    pParentRegion = pNodeRegion;
                    pParentNode = parentNode;
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
        GetCurrentNodeBlocks().Set(node, currentRegion);

        if (GetMapCompilerParameters().compilerSettings.includeDiagnosticInfo)
        {
            currentFunction.Print(DiagnosticString(node) + '\n');
        }
    }

    bool IRMapCompiler::TryMergeNodeRegion(const Node& node)
    {
        auto pRegion = GetCurrentNodeBlocks().Get(node);
        if (pRegion == nullptr)
        {
            return false;
        }

        const Node* pParentNode = GetUniqueParent(node);
        if (pParentNode == nullptr)
        {
            return false;
        }

        return TryMergeNodeRegions(*pParentNode, node);
    }

    bool IRMapCompiler::TryMergeNodeRegions(const Node& dest, const Node& src)
    {
        emitters::IRBlockRegion* pDestRegion = GetCurrentNodeBlocks().Get(dest);
        if (pDestRegion == nullptr)
        {
            return false;
        }
        return TryMergeNodeIntoRegion(pDestRegion, src);
    }

    bool IRMapCompiler::TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestRegion, const Node& src)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();

        emitters::IRBlockRegion* pSrcRegion = GetCurrentNodeBlocks().Get(src);
        if (pSrcRegion == nullptr || pSrcRegion == pDestRegion)
        {
            return false;
        }

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
            emitters::Variable* pVar = GetVariableForElement(element);
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
    llvm::Value* IRMapCompiler::LoadPortVariable(const InputPortBase& port)
    {
        return LoadPortElementVariable(port.GetInputElement(0)); // Note: this fails on scalar input variables
    }

    llvm::Value* IRMapCompiler::LoadPortElementVariable(const PortElementBase& element)
    {
        auto& currentFunction = GetModule().GetCurrentFunction();

        // Error: if we pass in a single element from a range, we need to use startindex as part of the key for looking up the element. In fact, we should have a separate map for vector port and scalar element variables...
        emitters::Variable* pVar = GetVariableForElement(element);
        llvm::Value* pVal = GetModule().EnsureEmitted(*pVar);
        if (pVar->IsScalar())
        {
            if (pVar->IsLiteral())
            {
                return pVal;
            }
            else if (pVar->IsInputArgument())
            {
                return pVal;
            }
            else
            {
                return currentFunction.Load(pVal);
            }
        }

        // Else return an element from a vector (unless it was in fact passed in by value)
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

    emitters::Variable* IRMapCompiler::GetPortElementVariable(const PortElementBase& element)
    {
        emitters::Variable* pVar = GetVariableForElement(element);
        if (pVar == nullptr)
        {
            throw emitters::EmitterException(emitters::EmitterError::notSupported, "Variable for output port not found");
        }
        if (pVar->IsScalar() && element.GetIndex() > 0)
        {
            throw emitters::EmitterException(emitters::EmitterError::vectorVariableExpected);
        }
        else if (element.GetIndex() >= pVar->Dimension())
        {
            throw emitters::EmitterException(emitters::EmitterError::indexOutOfRange);
        }

        return pVar;
    }

    emitters::Variable* IRMapCompiler::GetPortVariable(const InputPortBase& port)
    {
        return GetPortElementVariable(port.GetInputElement(0)); // Note: Potential error: scalar vars passed by value won't work here
    }
}
}
