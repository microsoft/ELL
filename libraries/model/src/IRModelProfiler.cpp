////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModelProfiler.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRModelProfiler.h"
#include "IRFunctionEmitter.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"
#include "LLVMUtilities.h"

// utilities
#include "UniqueId.h"

// stl
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <string>

namespace ell
{
namespace model
{
    //
    // NodeInfoEmitter
    //
    NodeInfoEmitter::NodeInfoEmitter(emitters::IRModuleEmitter& module, const Node* node, llvm::Value* nodeInfoPtr, llvm::StructType* nodeInfoType)
        : _module(&module), _node(node), _nodeInfoPtr(nodeInfoPtr), _nodeInfoType(nodeInfoType)
    {
    }

    void NodeInfoEmitter::Init(emitters::IRFunctionEmitter& function)
    {
        assert(_nodeInfoPtr != nullptr);

        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        // Allocate global storage for node info
        auto nodeName = utilities::to_string(_node->GetId());
        auto nodeTypeName = _node->GetRuntimeTypeName();

        // Add NodeInfo field global initialization
        auto namePtr = irBuilder.CreateInBoundsGEP(_nodeInfoType, _nodeInfoPtr, { emitter.Literal(0), emitter.Literal(0) });
        auto typePtr = irBuilder.CreateInBoundsGEP(_nodeInfoType, _nodeInfoPtr, { emitter.Literal(0), emitter.Literal(1) });

        function.Store(namePtr, function.Literal(nodeName));
        function.Store(typePtr, function.Literal(nodeTypeName));
    }

    //
    // PerformanceCountersEmitter
    //
    PerformanceCountersEmitter::PerformanceCountersEmitter(emitters::IRModuleEmitter& module, llvm::Value* performanceCountersPtr, llvm::StructType* performanceCountersType)
        : _module(&module), _performanceCountersPtr(performanceCountersPtr), _performanceCountersType(performanceCountersType)
    {
    }

    void PerformanceCountersEmitter::Init(emitters::IRFunctionEmitter& function)
    {
    }

    void PerformanceCountersEmitter::Start(emitters::IRFunctionEmitter& function, llvm::Value* startTime)
    {
        assert(_performanceCountersPtr != nullptr);

        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        _startTime = startTime;

        // Increment node entry counter
        auto countPtr = irBuilder.CreateInBoundsGEP(_performanceCountersType, _performanceCountersPtr, { emitter.Literal(0), emitter.Literal(0) });
        function.OperationAndUpdate(countPtr, emitters::TypedOperator::add, function.Literal<int64_t>(1));
    }

    void PerformanceCountersEmitter::End(emitters::IRFunctionEmitter& function, llvm::Value* endTime)
    {
        assert(_performanceCountersPtr != nullptr);

        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        // Compute time elapsed and increment total time counter
        auto elapsedTime = function.Operator(emitters::TypedOperator::subtractFloat, endTime, _startTime);
        auto totalTimePtr = irBuilder.CreateInBoundsGEP(_performanceCountersPtr, { emitter.Literal(0), emitter.Literal(1) }, "accumTime");
        function.OperationAndUpdate(totalTimePtr, emitters::TypedOperator::addFloat, elapsedTime);
    }

    void PerformanceCountersEmitter::Reset(emitters::IRFunctionEmitter& function)
    {
        assert(_performanceCountersPtr != nullptr);

        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        // Compute time elapsed and increment total time counter
        auto countPtr = irBuilder.CreateInBoundsGEP(_performanceCountersType, _performanceCountersPtr, { emitter.Literal(0), emitter.Literal(0) });
        auto totalTimePtr = irBuilder.CreateInBoundsGEP(_performanceCountersPtr, { emitter.Literal(0), emitter.Literal(1) });
        function.StoreZero(countPtr);
        function.StoreZero(totalTimePtr);
    }

    //
    // NodePerformanceEmitter
    //
    NodePerformanceEmitter::NodePerformanceEmitter(emitters::IRModuleEmitter& module, const Node* node, llvm::Value* nodeInfoPtr, llvm::Value* performanceCountersPtr, llvm::StructType* nodeInfoType, llvm::StructType* performanceCountersType)
        : _nodeInfoEmitter(module, node, nodeInfoPtr, nodeInfoType), _performanceCountersEmitter(module, performanceCountersPtr, performanceCountersType)
    {
    }

    void NodePerformanceEmitter::Init(emitters::IRFunctionEmitter& function)
    {
        _nodeInfoEmitter.Init(function);
        _performanceCountersEmitter.Init(function);
    }

    void NodePerformanceEmitter::Start(emitters::IRFunctionEmitter& function, llvm::Value* startTime)
    {
        _performanceCountersEmitter.Start(function, startTime);
    }

    void NodePerformanceEmitter::End(emitters::IRFunctionEmitter& function, llvm::Value* endTime)
    {
        _performanceCountersEmitter.End(function, endTime);
    }

    void NodePerformanceEmitter::Reset(emitters::IRFunctionEmitter& function)
    {
        _performanceCountersEmitter.Reset(function);
    }

    //
    // ModelProfiler
    //
    ModelProfiler::ModelProfiler()
        : _module(nullptr), _model(nullptr), _profilingEnabled(false), _nodeInfoType(nullptr), _performanceCountersType(nullptr)
    {
        // Emit functions
    }

    ModelProfiler::ModelProfiler(emitters::IRModuleEmitter& module, Model& model, bool enableProfiling)
        : _module(&module), _model(&model), _profilingEnabled(enableProfiling), _nodeInfoType(nullptr), _performanceCountersType(nullptr)
    {
        // Emit functions
    }

    void ModelProfiler::EmitInitialization()
    {
        if (_profilingEnabled)
        {
            assert(_module != nullptr);
            assert(_model != nullptr);

            _module->DeclarePrintf();
            CreateStructTypes();
            AllocateNodeData();
        }
    }

    void ModelProfiler::CreateStructTypes()
    {
        auto& context = _module->GetLLVMContext();

        auto int64Type = llvm::Type::getInt64Ty(context);
        auto doubleType = llvm::Type::getDoubleTy(context);
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        // NodeInfo struct fields
        emitters::NamedLLVMTypeList infoFields = { { "nodeName", int8PtrType }, { "nodeType", int8PtrType } };
        _nodeInfoType = _module->GetOrCreateStruct(GetNamespacePrefix() + "_NodeInfo", infoFields);
        _module->IncludeTypeInHeader(_nodeInfoType->getName());

        emitters::NamedLLVMTypeList countersFields = { { "count", int64Type }, { "totalTime", doubleType } };
        _performanceCountersType = _module->GetOrCreateStruct(GetNamespacePrefix() + "_PerformanceCounters", countersFields);
        _module->IncludeTypeInHeader(_performanceCountersType->getName());
    }

    void ModelProfiler::StartModel(emitters::IRFunctionEmitter& function)
    {
        if (!_profilingEnabled)
        {
            return;
        }

        auto startTime = CallGetCurrentTime(function);
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        assert(_modelPerformanceCountersArray != nullptr);
        auto modelPerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_modelPerformanceCountersArray, { emitter.Literal(0), emitter.Literal(0) });
        _modelPerformanceCounters = { *_module, modelPerformanceCountersPtr, _performanceCountersType };

        _modelPerformanceCounters.Init(function);
        _modelPerformanceCounters.Start(function, startTime);
    }

    void ModelProfiler::EndModel(emitters::IRFunctionEmitter& function)
    {
        if (!_profilingEnabled)
        {
            return;
        }

        auto endTime = CallGetCurrentTime(function);
        _modelPerformanceCounters.End(function, endTime);
    }

    void ModelProfiler::InitNode(emitters::IRFunctionEmitter& function, const Node& node)
    {
        if (!_profilingEnabled)
        {
            return;
        }
        auto& performanceCounters = GetPerformanceCountersForNode(node);
        auto& typePerformanceCounters = GetTypePerformanceCountersForNode(node);

        performanceCounters.Init(function);
        typePerformanceCounters.Init(function);
    }

    void ModelProfiler::StartNode(emitters::IRFunctionEmitter& function, const Node& node)
    {
        if (!_profilingEnabled)
        {
            return;
        }
        auto& performanceCounters = GetPerformanceCountersForNode(node);
        auto& typePerformanceCounters = GetTypePerformanceCountersForNode(node);

        auto startTime = CallGetCurrentTime(function);
        performanceCounters.Start(function, startTime);
        typePerformanceCounters.Start(function, startTime);
    }

    void ModelProfiler::EndNode(emitters::IRFunctionEmitter& function, const Node& node)
    {
        if (!_profilingEnabled)
        {
            return;
        }
        auto& performanceCounters = GetPerformanceCountersForNode(node);
        auto& typePerformanceCounters = GetTypePerformanceCountersForNode(node);

        auto endTime = CallGetCurrentTime(function);
        performanceCounters.End(function, endTime);
        typePerformanceCounters.End(function, endTime);
    }

    void ModelProfiler::EmitModelProfilerFunctions()
    {
        if (!_profilingEnabled)
        {
            return;
        }

        assert(_module != nullptr);
        assert(_model != nullptr);

        EmitGetModelPerformanceCountersFunction();
        EmitPrintModelProfilingInfoFunction();
        EmitResetModelProfilingInfoFunction();

        // EmitGetNumNodesFunction();
        EmitGetNodeInfoFunction();
        EmitGetNodePerformanceCountersFunction();
        EmitPrintNodeProfilingInfoFunction();
        EmitResetNodeProfilingInfoFunction();

        EmitGetNumNodeTypesFunction();
        EmitGetNodeTypeInfoFunction();
        EmitGetNodeTypePerformanceCountersFunction();
        EmitPrintNodeTypeProfilingInfoFunction();
        EmitResetNodeTypeProfilingInfoFunction();
    }

    void ModelProfiler::AllocateNodeData()
    {
        _modelPerformanceCountersArray = _module->GlobalArray(GetNamespacePrefix() + "_ModelPerformanceCountersArray", _performanceCountersType, 2);

        int numNodes = _model->Size();
        _nodeInfoArray = _module->GlobalArray(GetNamespacePrefix() + "_NodeInfoArray", _nodeInfoType, numNodes);
        _nodePerformanceCountersArray = _module->GlobalArray(GetNamespacePrefix() + "_NodePerformanceCountersArray", _performanceCountersType, numNodes);

        // Note: We're grossly overallocating global array for types
        _nodeTypeInfoArray = _module->GlobalArray(GetNamespacePrefix() + "_NodeTypeInfoArray", _nodeInfoType, numNodes);
        _nodeTypePerformanceCountersArray = _module->GlobalArray(GetNamespacePrefix() + "_NodeTypePerformanceCountersArray", _performanceCountersType, numNodes);
    }

    std::string ModelProfiler::GetNamespacePrefix() const
    {
        return _module->GetModuleName();
    }

    void ModelProfiler::EmitGetModelPerformanceCountersFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetModelPerformanceCounters", _performanceCountersType->getPointerTo());
        function.IncludeInHeader();

        auto performanceCountersPtr = irBuilder.CreateInBoundsGEP(_modelPerformanceCountersArray, { function.Literal(0), function.Literal(0) });
        function.Return(performanceCountersPtr);
        _module->EndFunction();
    }

    void ModelProfiler::EmitGetNumNodeTypesFunction()
    {
        auto& context = _module->GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        int numNodes = _nodeTypePerformanceCounters.size();

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetNumNodeTypes", int32Type);
        function.IncludeInHeader();

        function.Return(function.Literal(numNodes));
        _module->EndFunction();
    }

    // TODO: return nullptr if out of bounds (this is device-side code, and we may not be able to throw exceptions)
    void ModelProfiler::EmitGetNodeInfoFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        const emitters::NamedVariableTypeList parameters = { { "nodeIndex", emitters::VariableType::Int32 } };
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetNodeInfo", _nodeInfoType->getPointerTo(), parameters);
        function.IncludeInHeader();

        auto args = function.Arguments();
        auto nodeIndex = &(*args.begin());
        auto nodeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeInfoArray, { function.Literal(0), nodeIndex });
        function.Return(nodeInfoPtr);
        _module->EndFunction();
    }

    // TODO: return nullptr if out of bounds (this is device-side code, and we may not be able to throw exceptions)
    void ModelProfiler::EmitGetNodeTypeInfoFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        const emitters::NamedVariableTypeList parameters = { { "nodeIndex", emitters::VariableType::Int32 } };
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetNodeTypeInfo", _nodeInfoType->getPointerTo(), parameters);
        function.IncludeInHeader();

        auto args = function.Arguments();
        auto nodeIndex = &(*args.begin());
        auto nodeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeTypeInfoArray, { function.Literal(0), nodeIndex });
        function.Return(nodeInfoPtr);
        _module->EndFunction();
    }

    // TODO: return nullptr if out of bounds (this is device-side code, and we may not be able to throw exceptions)
    void ModelProfiler::EmitGetNodePerformanceCountersFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        const emitters::NamedVariableTypeList parameters = { { "nodeIndex", emitters::VariableType::Int32 } };
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetNodePerformanceCounters", _performanceCountersType->getPointerTo(), parameters);
        function.IncludeInHeader();

        auto args = function.Arguments();
        auto nodeIndex = &(*args.begin());
        auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodePerformanceCountersArray, { function.Literal(0), nodeIndex });
        function.Return(nodePerformanceCountersPtr);
        _module->EndFunction();
    }

    // TODO: return nullptr if out of bounds (this is device-side code, and we may not be able to throw exceptions)
    void ModelProfiler::EmitGetNodeTypePerformanceCountersFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& irBuilder = emitter.GetIRBuilder();
        const emitters::NamedVariableTypeList parameters = { { "nodeIndex", emitters::VariableType::Int32 } };

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_GetNodeTypePerformanceCounters", _performanceCountersType->getPointerTo(), parameters);
        function.IncludeInHeader();

        auto args = function.Arguments();
        auto nodeIndex = &(*args.begin());
        auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodeTypePerformanceCountersArray, { function.Literal(0), nodeIndex });
        function.Return(nodePerformanceCountersPtr);
        _module->EndFunction();
    }

    void ModelProfiler::EmitPrintModelProfilingInfoFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto& irBuilder = emitter.GetIRBuilder();

        auto voidType = llvm::Type::getVoidTy(context);
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_PrintModelProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();

        auto modelPerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_modelPerformanceCountersArray, { function.Literal(0), function.Literal(0) });

        // Print some statistics
        auto countPtr = irBuilder.CreateInBoundsGEP(modelPerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
        auto totalTimePtr = irBuilder.CreateInBoundsGEP(modelPerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
        function.Printf("Total time: %f ms\tcount: %d\n", { function.Load(totalTimePtr), function.Load(countPtr) });

        _module->EndFunction();
    }

    void ModelProfiler::EmitResetModelProfilingInfoFunction()
    {
        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto& irBuilder = emitter.GetIRBuilder();

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_ResetModelProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();

        auto modelPerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_modelPerformanceCountersArray, { function.Literal(0), function.Literal(0) });

        auto countPtr = irBuilder.CreateInBoundsGEP(modelPerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
        auto totalTimePtr = irBuilder.CreateInBoundsGEP(modelPerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
        function.StoreZero(countPtr);
        function.StoreZero(totalTimePtr);

        _module->EndFunction();
    }

    void ModelProfiler::EmitPrintNodeProfilingInfoFunction()
    {
        int numEmittedNodes = _nodePerformanceCounters.size();
        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto& irBuilder = emitter.GetIRBuilder();

        auto voidType = llvm::Type::getVoidTy(context);
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_PrintNodeProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();

        auto loop = function.ForLoop();
        loop.Begin(numEmittedNodes);
        {
            auto nodeIndex = loop.LoadIterationVariable();
            auto nodeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeInfoArray, { function.Literal(0), nodeIndex });
            auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodePerformanceCountersArray, { function.Literal(0), nodeIndex });

            // Print some stuff
            auto namePtr = irBuilder.CreateGEP(nodeInfoPtr, { emitter.Literal(0), emitter.Literal(0) });
            auto typePtr = irBuilder.CreateGEP(nodeInfoPtr, { emitter.Literal(0), emitter.Literal(1) });

            auto countPtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
            auto totalTimePtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
            function.Printf("Node[%s]:\ttype: %s\ttime: %f ms\tcount: %d\n", { function.Load(namePtr), function.Load(typePtr), function.Load(totalTimePtr), function.Load(countPtr) });
        }
        loop.End();

        _module->EndFunction();
    }

    void ModelProfiler::EmitPrintNodeTypeProfilingInfoFunction()
    {
        int numEmittedNodeTypes = _nodeTypePerformanceCounters.size();
        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto& irBuilder = emitter.GetIRBuilder();

        auto voidType = llvm::Type::getVoidTy(context);
        auto function = _module->BeginFunction(GetNamespacePrefix() + "_PrintNodeTypeProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();

        auto loop = function.ForLoop();
        loop.Begin(numEmittedNodeTypes);
        {
            auto nodeIndex = loop.LoadIterationVariable();
            auto nodeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeTypeInfoArray, { function.Literal(0), nodeIndex });
            auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodeTypePerformanceCountersArray, { function.Literal(0), nodeIndex });

            // Print some stuff
            auto typePtr = irBuilder.CreateGEP(nodeInfoPtr, { emitter.Literal(0), emitter.Literal(1) });

            auto countPtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
            auto totalTimePtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
            function.Printf("type: %s\ttime: %f ms\tcount: %d\n", { function.Load(typePtr), function.Load(totalTimePtr), function.Load(countPtr) });
        }
        loop.End();

        _module->EndFunction();
    }

    void ModelProfiler::EmitResetNodeProfilingInfoFunction()
    {
        int numEmittedNodes = _nodePerformanceCounters.size();

        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_ResetNodeProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();
        auto& irBuilder = emitter.GetIRBuilder();

        auto nodeLoop = function.ForLoop();
        nodeLoop.Begin(numEmittedNodes);
        {
            auto nodeIndex = nodeLoop.LoadIterationVariable();
            auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodePerformanceCountersArray, { function.Literal(0), nodeIndex });

            auto countPtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
            auto totalTimePtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
            function.StoreZero(countPtr);
            function.StoreZero(totalTimePtr);
        }
        nodeLoop.End();

        _module->EndFunction();
    }

    void ModelProfiler::EmitResetNodeTypeProfilingInfoFunction()
    {
        int numEmittedNodes = _nodeTypePerformanceCounters.size();

        auto& emitter = _module->GetIREmitter();
        auto& context = _module->GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        auto function = _module->BeginFunction(GetNamespacePrefix() + "_ResetNodeTypeProfilingInfo", voidType);
        function.IncludeInHeader();
        function.IncludeInSwigInterface();
        auto& irBuilder = emitter.GetIRBuilder();

        auto loop = function.ForLoop();
        loop.Begin(numEmittedNodes);
        {
            auto nodeIndex = loop.LoadIterationVariable();
            auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodeTypePerformanceCountersArray, { function.Literal(0), nodeIndex });

            auto countPtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(0) });
            auto totalTimePtr = irBuilder.CreateInBoundsGEP(nodePerformanceCountersPtr, { function.Literal(0), function.Literal(1) });
            function.StoreZero(countPtr);
            function.StoreZero(totalTimePtr);
        }
        loop.End();

        _module->EndFunction();
    }

    NodePerformanceEmitter& ModelProfiler::GetPerformanceCountersForNode(const Node& node)
    {
        assert(_profilingEnabled);
        auto it = _nodePerformanceCounters.find(&node);
        if (it == _nodePerformanceCounters.end())
        {
            auto& emitter = _module->GetIREmitter();
            auto& irBuilder = emitter.GetIRBuilder();
            int nodeIndex = _nodePerformanceCounters.size();

            auto nodeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeInfoArray, { emitter.Literal(0), emitter.Literal(nodeIndex) });
            auto nodePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodePerformanceCountersArray, { emitter.Literal(0), emitter.Literal(nodeIndex) });

            NodePerformanceEmitter performanceCounters(*_module, &node, nodeInfoPtr, nodePerformanceCountersPtr, _nodeInfoType, _performanceCountersType);
            _nodePerformanceCounters[&node] = performanceCounters;
        }

        return _nodePerformanceCounters[&node];
    }

    NodePerformanceEmitter& ModelProfiler::GetTypePerformanceCountersForNode(const Node& node)
    {
        assert(_profilingEnabled);
        auto nodeType = node.GetRuntimeTypeName();
        auto it = _nodeTypePerformanceCounters.find(nodeType);
        if (it == _nodeTypePerformanceCounters.end())
        {
            auto& emitter = _module->GetIREmitter();
            auto& irBuilder = emitter.GetIRBuilder();
            int nodeIndex = _nodeTypePerformanceCounters.size();

            auto nodeTypeInfoPtr = irBuilder.CreateInBoundsGEP(_nodeTypeInfoArray, { emitter.Literal(0), emitter.Literal(nodeIndex) });
            auto nodeTypePerformanceCountersPtr = irBuilder.CreateInBoundsGEP(_nodeTypePerformanceCountersArray, { emitter.Literal(0), emitter.Literal(nodeIndex) });

            NodePerformanceEmitter performanceCounters(*_module, &node, nodeTypeInfoPtr, nodeTypePerformanceCountersPtr, _nodeInfoType, _performanceCountersType);
            _nodeTypePerformanceCounters[nodeType] = performanceCounters;
        }

        return _nodeTypePerformanceCounters[nodeType];
    }

    llvm::Value* ModelProfiler::CallGetCurrentTime(emitters::IRFunctionEmitter& function)
    {
        auto time = _module->GetRuntime().GetCurrentTime(function);
        return time;
    }
}
}
