////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMapCompiler.h (model)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRCompiledMap.h"
#include "MapCompiler.h"

// emitters
#include "EmitterException.h"
#include "IRBlockRegion.h"
#include "IRModuleEmitter.h"
#include "NodeMap.h"

// model
#include "InputPort.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// stl
#include <string>

namespace ell
{
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
        IRMapCompiler(const MapCompilerParameters& settings);

        /// <summary> Compile a map into a CompiledMap </summary>
        ///
        /// <param name="map"> The map to compile </param>
        /// <param name="functionName"> The name of the function to evaluate the map </param>
        IRCompiledMap Compile(DynamicMap map, const std::string& functionName = "predict");

        emitters::CompilerParameters GetCompilerParameters() const { return GetModule().GetCompilerParameters(); }

        //
        // Routines useful to Node implementers
        //

        emitters::IRModuleEmitter& GetModule() { return _moduleEmitter; }
        const emitters::IRModuleEmitter& GetModule() const { return _moduleEmitter; }

        /// <summary> Ensure that variable for the outport port referenced by this input port has been declared in IR </summary>
        llvm::Value* EnsurePortEmitted(const model::InputPortBase& port);

        /// <summary> Ensure that variable for the given port has been declared in IR </summary>
        llvm::Value* EnsurePortEmitted(const model::OutputPortBase& port);

        /// <summary> Ensure that variable for the given port element has been declared in IR </summary>
        llvm::Value* EnsurePortElementEmitted(const model::PortElementBase& element);

        /// <summary> Load the variable for the outport port referenced by this input port </summary>
        llvm::Value* LoadPortVariable(const model::InputPortBase& port);

        /// <summary> Ensure that the variable for this outport port element is loaded into a register. This will automatically
        /// dereference any pointers it needs to. </summary>
        llvm::Value* LoadPortElementVariable(const model::PortElementBase& element);

        emitters::Variable* GetPortVariable(const model::InputPortBase& port);
        emitters::Variable* GetPortElementVariable(const PortElementBase& element);

        /// <summary> Creates a new BlockRegion for the node </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        void NewNodeRegion(const model::Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the previous block </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeNodeRegion(const model::Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the BlockRegion from another node </summary>
        ///
        /// <param name="dest"> The node we merging into </param>
        /// <param name="src"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeNodeRegions(const model::Node& dest, const model::Node& src) override;

        /// <summary>
        /// Returns the `IRBlockRegion` that computes the given element's value, if
        /// it is legal to merge that block into the block for the current node.
        /// Otherwise, returns `nullptr`.
        /// </summary>
        ///
        /// <param name="element"> The port element whose block we want to merge into. </param>
        /// <returns> The `IRBlockRegion` that computes `element` if that block is mergeable, `nullptr` otherwise. </returns>
        emitters::IRBlockRegion* GetMergeableNodeRegion(const model::PortElementBase& element);

        llvm::LLVMContext& GetLLVMContext();

    protected:
        virtual void OnBeginCompileNode(const model::Node& node) override;
        virtual void OnEndCompileNode(const model::Node& node) override;
        virtual void PushScope() override;
        virtual void PopScope() override;
        virtual emitters::ModuleEmitter* GetModuleEmitter() override { return &_moduleEmitter; }

    private:
        void EnsureValidMap(DynamicMap& map);
        NodeMap<emitters::IRBlockRegion*>& GetCurrentNodeBlocks();
        const model::Node* GetUniqueParent(const model::Node& node);

        bool TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestination, const model::Node& src);

        emitters::IRModuleEmitter _moduleEmitter;

        // stack of node regions
        std::vector<NodeMap<emitters::IRBlockRegion*>> _nodeRegions;
    };
}
}
