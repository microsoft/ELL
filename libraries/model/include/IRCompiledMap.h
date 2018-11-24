////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRCompiledMap.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompiledMap.h"
#include "IRModelProfiler.h"
#include "InputNode.h"
#include "Map.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// emitters
#include "IRExecutionEngine.h"
#include "IRModuleEmitter.h"
#include "ModuleEmitter.h"

// utilities
#include "Boolean.h"
#include "TypeName.h"

// stl
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A map that can be compiled </summary>
    class IRCompiledMap : public CompiledMap
    {
        using Boolean = utilities::Boolean;

    public:
        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> The compiled map being moved. </param>
        IRCompiledMap(IRCompiledMap&& other);

        ~IRCompiledMap() override = default;

        /// <summary> Output the compiled model to the given file </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        void WriteCode(const std::string& filePath) const override;

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        /// <param name="format"> The format to write out </param>
        void WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format) const override;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="filePath"> The path to the file to write </param>
        /// <param name="format"> The format to write out (cHeader or Swig interface) </param>
        void WriteCodeHeader(const std::string& filePath, emitters::ModuleOutputFormat format) const override;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="streawm"> The stream to write the prototype to </param>
        /// <param name="format"> The format to write out (cHeader or Swig interface) </param>
        void WriteCodeHeader(std::ostream& stream, emitters::ModuleOutputFormat format) const override;

        /// <summary> Output the compiled model to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format to write out </param>
        void WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format) const override;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <returns> A string with the function prototype </returns>
        std::string GetCodeHeaderString() const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IRCompiledMap"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Can this compiled map be used? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        bool IsValid() const override;

        /// <summary> Gets a reference to the underlying IRModuleEmitter. </summary>
        ///
        /// <returns> Reference to an IRModuleEmitter. </returns>
        emitters::IRModuleEmitter& GetModule() { return *_module; }

        /// <summary> Gets a reference to the underlying jitter. </summary>
        ///
        /// <returns> The jitter. </returns>
        emitters::IRExecutionEngine& GetJitter();

        //
        // Node profiling support
        //

        /// <summary> Get a pointer to the performance counters struct for the whole model. </summary>
        PerformanceCounters* GetModelPerformanceCounters();

        /// <summary> Print a summary of the performance for the model. </summary>
        void PrintModelProfilingInfo();

        /// <summary> Reset the performance summary for the model to zero. </summary>
        void ResetModelProfilingInfo();

        /// <summary> Get the number of nodes that have profiling information. </summary>
        int GetNumProfiledNodes();

        /// <summary> Get a pointer to the info struct for a node. </summary>
        ///
        /// <param name="nodeIndex"> the index of the node. </param>
        NodeInfo* GetNodeInfo(int nodeIndex);

        /// <summary> Get a pointer to the performance counters struct for a node. </summary>
        ///
        /// <param name="nodeIndex"> the index of the node. </param>
        PerformanceCounters* GetNodePerformanceCounters(int nodeIndex);

        /// <summary> Print a summary of the performance for the nodes. </summary>
        void PrintNodeProfilingInfo();

        /// <summary> Reset the performance counters for all the nodes to zero. </summary>
        void ResetNodeProfilingInfo();

        /// <summary> Get the number of node types that have profiling information. </summary>
        int GetNumProfiledNodeTypes();

        /// <summary> Get a pointer to the info struct for a node type. </summary>
        ///
        /// <param name="nodeIndex"> the index of the node type. </param>
        NodeInfo* GetNodeTypeInfo(int nodeIndex);

        /// <summary> Get a pointer to the aggregated performance counters struct for a node type. </summary>
        ///
        /// <param name="nodeIndex"> the index of the node type. </param>
        PerformanceCounters* GetNodeTypePerformanceCounters(int nodeIndex);

        /// <summary> Get a pointer to the named global array. </summary>
        ///
        /// <param name="name"> name of the global. </param>
        template <typename ElementType>
        ElementType* GetGlobalValuePointer(const std::string& name);

        /// <summary> Print a summary of the performance for the node types. </summary>
        void PrintNodeTypeProfilingInfo();

        /// <summary> Reset the performance counters for all the node types to zero. </summary>
        void ResetNodeTypeProfilingInfo();

        //
        // Low-level region profiling support
        //

        /// <summary> Get the number of regions that have profiling information. </summary>
        int GetNumProfileRegions();

        /// <summary> Get a pointer to the info struct for a region. </summary>
        ///
        /// <param name="regionIndex"> the index of the region. </param>
        emitters::ProfileRegionInfo* GetRegionProfilingInfo(int regionIndex);

        /// <summary> Reset the performance summary for the model to zero. </summary>
        void ResetRegionProfilingInfo();

        //
        // Just-in-time compilation functions
        //

        /// <summary> Force jitting to finish so you can time execution without jit cost. </summary>
        void FinishJitting() const;

        /// <summary> Set a context object to use in the predict call </summary>
        void SetContext(void* context) { _context = context; }

        /// <summary> Get the context object to use in the predict call </summary>
        void* GetContext() const { return _context; }

    protected:
        void WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options) const;
        void WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options) const;

        void SetNodeInput(model::InputNode<bool>* node, const std::vector<bool>& inputValues) const override;
        void SetNodeInput(model::InputNode<int>* node, const std::vector<int>& inputValues) const override;
        void SetNodeInput(model::InputNode<int64_t>* node, const std::vector<int64_t>& inputValues) const override;
        void SetNodeInput(model::InputNode<float>* node, const std::vector<float>& inputValues) const override;
        void SetNodeInput(model::InputNode<double>* node, const std::vector<double>& inputValues) const override;

        std::vector<bool> ComputeBoolOutput(const model::PortElementsBase& outputs) const override;
        std::vector<int> ComputeIntOutput(const model::PortElementsBase& outputs) const override;
        std::vector<int64_t> ComputeInt64Output(const model::PortElementsBase& outputs) const override;
        std::vector<float> ComputeFloatOutput(const model::PortElementsBase& outputs) const override;
        std::vector<double> ComputeDoubleOutput(const model::PortElementsBase& outputs) const override;

    private:
        friend class IRMapCompiler;

        IRCompiledMap(Map map, const std::string& functionName, const MapCompilerOptions& options, std::unique_ptr<emitters::IRModuleEmitter> module, bool verifyJittedModule);

        void EnsureExecutionEngine() const;
        void SetComputeFunction() const;
        template <typename InputType>
        void SetComputeFunctionForInputType() const;

        template <typename InputType>
        using ComputeFunction = std::function<void(void*, const InputType*)>;

        std::string _moduleName = "ELL";
        std::unique_ptr<emitters::IRModuleEmitter> _module;

        mutable std::unique_ptr<emitters::IRExecutionEngine> _executionEngine;
        bool _verifyJittedModule = false;
        void* _context = nullptr;

        template <typename T>
        using Vector = std::vector<std::conditional_t<std::is_same_v<bool, T>, Boolean, T>>;

        // Only one of the entries in each of these tuples is active, depending on the input and output types of the map
        mutable bool _computeFunctionDefined;
        mutable std::tuple<ComputeFunction<bool>, ComputeFunction<int>, ComputeFunction<int64_t>, ComputeFunction<float>, ComputeFunction<double>> _computeInputFunction;
        mutable std::tuple<Vector<bool>, Vector<int>, Vector<int64_t>, Vector<float>, Vector<double>> _cachedOutput;
    };
} // namespace model
} // namespace ell

#include "../tcc/IRCompiledMap.tcc"
