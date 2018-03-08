////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMapCompiler.h (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRCompiledMap.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelOptimizer.h"
#include "Node.h"
#include "NodeMap.h"
#include "OutputPort.h"
#include "PortElements.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRBlockRegion;
    class IRModuleEmitter;
}

namespace model
{
    /// <summary> Compiles ELL Models to LLVM IR </summary>
    class IRMapCompiler : public MapCompiler
    {
    public:
        //
        // User-level API
        //

        /// <summary> Create a compiler to produce an LLVM module with the default name </summary>
        IRMapCompiler();

        /// <summary> Create a compiler to produce an LLVM module with the default name and the given parameters </summary>
        IRMapCompiler(const MapCompilerOptions& settings);

        /// <summary> Compile a map into a CompiledMap </summary>
        ///
        /// <param name="map"> The map to compile </param>
        /// <param name="functionName"> The name of the function to evaluate the map </param>
        IRCompiledMap Compile(Map map);

        /// <summary> Gets the compiler parameters being used by the IR emitter. </summary>
        ///
        /// <returns> The CompilerOptions struct used by the IR emitter to control code generation. </returns>
        const emitters::CompilerOptions& GetCompilerOptions() const { return GetModule().GetCompilerOptions(); }

        ModelOptimizer& GetOptimizer() { return _optimizer; }
        //
        // Routines useful to Node implementers
        //

        /// <summary> Returns the IRModuleEmitter associated with this map. </summary>
        ///
        /// <returns> The IRModuleEmitter associated with this map. </returns>
        emitters::IRModuleEmitter& GetModule() { return _moduleEmitter; }

        /// <summary> Returns the IRModuleEmitter associated with this map. </summary>
        ///
        /// <returns> The IRModuleEmitter associated with this map. </returns>
        const emitters::IRModuleEmitter& GetModule() const { return _moduleEmitter; }

        /// <summary> Ensure that variable for the outport port referenced by this input port has been declared in IR </summary>
        ///
        /// <param name="port"> The port to ensure is emitted. </param>
        /// <returns> The LLVM Value object corresponding to the port. </returns>
        llvm::Value* EnsurePortEmitted(const InputPortBase& port);

        /// <summary> Ensure that variable for the given port has been declared in IR </summary>
        ///
        /// <param name="port"> The port to ensure is emitted. </param>
        /// <returns> The LLVM Value object corresponding to the port. </returns>
        llvm::Value* EnsurePortEmitted(const OutputPortBase& port);

        /// <summary> Ensure that variable for the given port has been declared in IR </summary>
        ///
        /// <param name="port"> The port to ensure is emitted. </param>
        /// <param name="initialValue"> The value to initialize the output buffer with. </param>
        ///
        /// <returns> The LLVM Value object corresponding to the port. </returns>
        template <typename ValueType>
        llvm::Value* EnsurePortEmitted(const OutputPortBase& port, ValueType initialValue);

        /// <summary> Ensure that variable for the given port element has been declared in IR </summary>
        ///
        /// <param name="port"> The port elements to ensure are emitted. </param>
        /// <returns> The LLVM Value object corresponding to the elements. </returns>
        llvm::Value* EnsurePortElementEmitted(const PortElementBase& element);

        /// <summary> Load the variable for the outport port referenced by this input port </summary>
        ///
        /// <param name="port"> The port to load into a register. </param>
        /// <returns> The LLVM Value object corresponding to the port. </returns>
        llvm::Value* LoadPortVariable(const InputPortBase& port);

        /// <summary>
        /// Ensure that the variable for this outport port element is loaded into a register. This will automatically
        /// dereference any pointers it needs to.
        /// </summary>
        ///
        /// <param name="port"> The port element to load into a register. </param>
        /// <returns> The LLVM Value object corresponding to the port element. </returns>
        llvm::Value* LoadPortElementVariable(const PortElementBase& element);

        /// <summary> Creates a new BlockRegion for the node </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        void NewNodeRegion(const Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the previous block </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        bool TryMergeNodeRegion(const Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the BlockRegion from another node </summary>
        ///
        /// <param name="dest"> The node we merging into </param>
        /// <param name="src"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        bool TryMergeNodeRegions(const Node& dest, const Node& src) override;

        /// <summary>
        /// Returns the `IRBlockRegion` that computes the given element's value, if
        /// it is legal to merge that block into the block for the current node.
        /// Otherwise, returns `nullptr`.
        /// </summary>
        ///
        /// <param name="element"> The port element whose block we want to merge into. </param>
        /// <returns> The `IRBlockRegion` that computes `element` if that block is mergeable, `nullptr` otherwise. </returns>
        emitters::IRBlockRegion* GetMergeableNodeRegion(const PortElementBase& element);

        /// <summary> Gets a reference to the underlying llvm context. </summary>
        ///
        /// <returns> Reference to the underlying llvm context. </returns>
        llvm::LLVMContext& GetLLVMContext();

        /// <summary> Gets the namespace string used to prefix emitted map-specific runtime functions. </summary>
        ///
        /// <returns> The namespace prefix for the emitted module. </returns>
        std::string GetNamespacePrefix() const;

    protected:
        void OnBeginCompileModel(const Model& model) override;
        void OnEndCompileModel(const Model& model) override;
        void OnBeginCompileNode(const Node& node) override;
        void OnEndCompileNode(const Node& node) override;
        void PushScope() override;
        void PopScope() override;
        emitters::ModuleEmitter* GetModuleEmitter() override { return &_moduleEmitter; }
        void EnsureValidMap(Map& map);
        virtual std::string GetPredictFunctionName() const;
        virtual void EmitModelAPIFunctions(const Map& map);
        emitters::Variable* GetPortVariable(const InputPortBase& port);
        emitters::Variable* GetPortElementVariable(const PortElementBase& element);

        emitters::IRModuleEmitter _moduleEmitter;
        // Profiler object for model
        ModelProfiler _profiler;
        ModelOptimizer _optimizer;

    private:
        NodeMap<emitters::IRBlockRegion*>& GetCurrentNodeBlocks();
        const Node* GetUniqueParent(const Node& node);
        bool TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestination, const Node& src);

        void EmitGetInputSizeFunction(const Map& map);
        void EmitGetOutputSizeFunction(const Map& map);
        void EmitGetNumNodesFunction(const Map& map);

        void EmitShapeEnum();
        void EmitGetInputShapeFunction(const Map& map);
        void EmitGetOutputShapeFunction(const Map& map);
        void EmitShapeConditionals(emitters::IRFunctionEmitter& fn, std::vector<InputShape> shapes);

        // stack of node regions
        std::vector<NodeMap<emitters::IRBlockRegion*>> _nodeRegions;
    };
}
}

#include "../tcc/IRMapCompiler.tcc"
