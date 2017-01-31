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
    class IRMapCompiler : public MapCompiler, public emitters::IRModuleEmitter
    {
    public:
        /// <summary> Create a compiler to produce an LLVM module with the default name </summary>
        IRMapCompiler();

        /// <summary> Create a compiler to produce an LLVM module with the given name </summary>
        IRMapCompiler(const std::string& moduleName);

        //
        // Routines for Node implementers
        //

        /// <summary> Ensure that variable for the given port has been declared in IR </summary>
        llvm::Value* EnsureEmitted(model::OutputPortBase* pPort);
        using emitters::IRModuleEmitter::EnsureEmitted;

        /// <summary> Ensure that variable for the given port element has been declared in IR </summary>
        llvm::Value* EnsureEmitted(model::PortElementBase& element);

        /// <summary> Ensure that variable for the outport port referenced by this input port has been declared in IR </summary>
        llvm::Value* EnsureEmitted(model::InputPortBase* pPort);

        /// <summary> Ensure that the variable for this outport port element is loaded into a register. SThis will automatically
        /// dereference any pointers it needs to. </summary>
        llvm::Value* LoadVariable(const model::PortElementBase& element);

        /// <summary> Load the variable for the outport port referenced by this input port </summary>
        llvm::Value* LoadVariable(model::InputPortBase* pPort);

        /// <summary> Creates a new BlockRegion for the node </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        void NewBlockRegion(const model::Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the previous block </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeRegion(const model::Node& node) override;

        /// <summary> Tries to merge the BlockRegion for the current node into the BlockRegion from another node </summary>
        ///
        /// <param name="dest"> The node we merging into </param>
        /// <param name="src"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeRegions(const model::Node& dest, const model::Node& src) override;

        /// <summary>
        /// Returns the `IRBlockRegion` that computes the given element's value, if
        /// it is legal to merge that block into the block for the current node.
        /// Otherwise, returns `nullptr`.
        /// </summary>
        ///
        /// <param name="element"> The port element whose block we want to merge into. </param>
        /// <returns> The `IRBlockRegion` that computes `element` if that block is mergeable, `nullptr` otherwise. </returns>
        emitters::IRBlockRegion* GetMergeableRegion(const model::PortElementBase& element);

    protected:
        virtual void OnBeginCompileNode(const model::Node& node) override;
        virtual void OnEndCompileNode(const model::Node& node) override;

    private:
        const model::Node* GetUniqueParent(const model::Node& node);

        bool TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestination, const model::Node& src);

        NodeMap<emitters::IRBlockRegion*> _nodeBlocks;
    };
}
}
