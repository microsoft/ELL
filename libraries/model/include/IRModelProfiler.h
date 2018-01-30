////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModelProfiler.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Node.h"

// emitters
#include "EmitterTypes.h"

// llvm
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

// stl
#include <string>

// External API for profiling functions
extern "C" {

/// <summary> A struct that holds information about a node. </summary>
struct NodeInfo
{
    const char* nodeName;
    const char* nodeType;
};

/// <summary> A struct that holds summary information about a node's runtime performance </summary>
struct PerformanceCounters
{
    int count;
    double totalTime;
};
}

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
    class IRModuleEmitter;
}

namespace model
{
    // import NodeInfo and PerformanceCounters into our namespace
    using ::NodeInfo;
    using ::PerformanceCounters;

    /// <summary> A utility class that emits IR to populate NodeInfo structs. </summary>
    class NodeInfoEmitter
    {
    public:
        // Note: the default constructor is only necessary because we store instances in a std::map
        NodeInfoEmitter() = default;

    private:
        friend class NodePerformanceEmitter;

        NodeInfoEmitter(emitters::IRModuleEmitter& module, const Node* node, llvm::Value* nodeInfoPtr, llvm::StructType* nodeInfoType);
        void Init(emitters::IRFunctionEmitter& function);

        emitters::IRModuleEmitter* _module = nullptr;
        const Node* _node = nullptr;

        llvm::Value* _nodeInfoPtr = nullptr;
        llvm::StructType* _nodeInfoType = nullptr;
    };

    /// <summary> A utility class that emits IR to populate PerformanceCounters structs. </summary>
    class PerformanceCountersEmitter
    {
    public:
        // Note: the default constructor is only necessary because we store instances in a std::map
        PerformanceCountersEmitter() = default;

    private:
        friend class ModelProfiler;
        friend class NodePerformanceEmitter;

        PerformanceCountersEmitter(emitters::IRModuleEmitter& module, llvm::Value* performanceCountersPtr, llvm::StructType* performanceCountersType);
        void Init(emitters::IRFunctionEmitter& function);
        void Start(emitters::IRFunctionEmitter& function, llvm::Value* startTime);
        void End(emitters::IRFunctionEmitter& function, llvm::Value* startTime);
        void Reset(emitters::IRFunctionEmitter& function);

        emitters::IRModuleEmitter* _module = nullptr;
        llvm::Value* _performanceCountersPtr = nullptr;
        llvm::StructType* _performanceCountersType = nullptr;

        // Temporary value used during processing
        llvm::Value* _startTime = nullptr;
    };

    /// <summary> A utility class that holds a NodeInfoEmitter and a PerformanceCounterEmitter. </summary>
    class NodePerformanceEmitter
    {
    public:
        // Note: the default constructor is only necessary because we store instances in a std::map
        NodePerformanceEmitter() = default;

    private:
        void Init(emitters::IRFunctionEmitter& function);
        void Start(emitters::IRFunctionEmitter& function, llvm::Value* startTime);
        void End(emitters::IRFunctionEmitter& function, llvm::Value* endTime);
        void Reset(emitters::IRFunctionEmitter& function);

        friend class ModelProfiler;

        NodePerformanceEmitter(emitters::IRModuleEmitter& module, const Node* node, llvm::Value* nodeInfoPtr, llvm::Value* NodePerformanceEmitterPtr, llvm::StructType* nodeInfoType, llvm::StructType* NodePerformanceEmitterType);

        // emitters for info and perf counters
        NodeInfoEmitter _nodeInfoEmitter;
        PerformanceCountersEmitter _performanceCountersEmitter;
    };

    /// <summary> A class that manages model-profiling code generation. </summary>
    class ModelProfiler
    {
    public:
        /// <summary> Constructor </summary>
        ModelProfiler();

        /// <summary> Constructor </summary>
        ///
        /// <param name="module"> The `IRModuleEmitter` to compile the model profiling information into. </param>
        /// <param name="model"> The model to profile </param>
        /// <param name="enableProfiling"> Indicates whether profiling should be enabled for this model. </param>
        ModelProfiler(emitters::IRModuleEmitter& module, Model& model, bool enableProfiling);

        /// <summary> Indicates if profiling is enabled. </summary>
        ///
        /// <returns> true if profiling is enabled, false if disabled. </returns>
        bool IsProfilingEnabled() const { return _profilingEnabled; }

        /// <summary> Emit static initialization code to allocate and initialize info and perf counter data. </summary>
        void EmitInitialization();

        /// <summary> Emit performance code at beginning of model predictor function. </summary>
        ///
        /// <param name="function"> The model function. </param>
        /// <param name="startTime"> The time that the model function started. </param>
        void StartModel(emitters::IRFunctionEmitter& function);

        /// <summary> Emit performance code at end of model predictor function. </summary>
        ///
        /// <param name="function"> The model function. </param>
        /// <param name="startTime"> The time that the model function exited. </param>
        void EndModel(emitters::IRFunctionEmitter& function);

        /// <summary> Emit static node performance initialization code at the beginning of a node. </summary>
        ///
        /// <param name="function"> The model function. </param>
        /// <param name="node"> The node being evaluated. </param>
        void InitNode(emitters::IRFunctionEmitter& function, const Node& node);

        /// <summary> Emit node performance start code at the beginning of a node's execution. </summary>
        ///
        /// <param name="function"> The model function. </param>
        /// <param name="node"> The node being evaluated. </param>
        /// <param name="startTime"> The time that the node evaluation started. </param>
        void StartNode(emitters::IRFunctionEmitter& function, const Node& node);

        /// <summary> Emit node performance start code at the end of a node's execution. </summary>
        ///
        /// <param name="function"> The model function. </param>
        /// <param name="node"> The node being evaluated. </param>
        /// <param name="endTime"> The time that the node evaluation ended. </param>
        void EndNode(emitters::IRFunctionEmitter& function, const Node& node);

        /// <summary> Emit the runtime API functions for querying model performance. </summary>
        void EmitModelProfilerFunctions();

    private:
        void CreateStructTypes();
        void AllocateNodeData();
        std::string GetNamespacePrefix() const;

        NodePerformanceEmitter& GetPerformanceCountersForNode(const Node& node);
        NodePerformanceEmitter& GetTypePerformanceCountersForNode(const Node& node);

        void EmitGetNumNodeTypesFunction();

        void EmitGetModelPerformanceCountersFunction();
        void EmitPrintModelProfilingInfoFunction();
        void EmitResetModelProfilingInfoFunction();

        void EmitGetNodeInfoFunction();
        void EmitGetNodePerformanceCountersFunction();
        void EmitPrintNodeProfilingInfoFunction();
        void EmitResetNodeProfilingInfoFunction();

        void EmitGetNodeTypeInfoFunction();
        void EmitGetNodeTypePerformanceCountersFunction();
        void EmitPrintNodeTypeProfilingInfoFunction();
        void EmitResetNodeTypeProfilingInfoFunction();

        llvm::Value* CallGetCurrentTime(emitters::IRFunctionEmitter& function);

        emitters::IRModuleEmitter* _module = nullptr;
        Model* _model = nullptr;
        bool _profilingEnabled = false;

        llvm::StructType* _nodeInfoType = nullptr;
        llvm::StructType* _performanceCountersType = nullptr;

        llvm::GlobalVariable* _modelPerformanceCountersArray = nullptr;

        llvm::GlobalVariable* _nodeInfoArray = nullptr;
        llvm::GlobalVariable* _nodePerformanceCountersArray = nullptr;

        llvm::GlobalVariable* _nodeTypeInfoArray = nullptr;
        llvm::GlobalVariable* _nodeTypePerformanceCountersArray = nullptr;

        // Performance counter emitters for model
        PerformanceCountersEmitter _modelPerformanceCounters;

        // Performance counter emitters for nodes
        std::map<const Node*, NodePerformanceEmitter> _nodePerformanceCounters;

        // Aggregate performance counter emitters for node types
        std::map<std::string, NodePerformanceEmitter> _nodeTypePerformanceCounters;
    };
}
}
