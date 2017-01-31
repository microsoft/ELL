////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompiler.h (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "EmitterTypes.h"
#include "ModuleEmitter.h"
#include "Variable.h"

// model
#include "DynamicMap.h"
#include "Model.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// stl
#include <string>
#include <unordered_map>

namespace ell
{
namespace model
{
    /// <summary> Abstract base class for ELL model compilers </summary>
    class MapCompiler
    {
    public:
        virtual ~MapCompiler() = default;

        /// <summary> Compile the map into a function with the given name </summary>
        ///
        /// <param name="map"> The map to compile </param>
        /// <param name="functionName"> The name of the function to create </param>
        void CompileMap(DynamicMap& map, const std::string& functionName);

        //
        // Routines for Node implementers
        //

        /// <summary> Creates a new BlockRegion for the node </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        virtual void NewBlockRegion(const Node& node) = 0;

        /// <summary> Tries to merge the BlockRegion for the current node into the previous block </summary>
        ///
        /// <param name="node"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeRegion(const Node& node) = 0;

        /// <summary> Tries to merge the BlockRegion for the current node into the BlockRegion from another node </summary>
        ///
        /// <param name="dest"> The node we merging into </param>
        /// <param name="src"> The node we're compiling </param>
        /// <returns> `true` if the regions were merged </returns>
        virtual bool TryMergeRegions(const Node& dest, const Node& src) = 0;

        /// <summary> Get the variable for output port </summary>
        emitters::Variable* GetVariableFor(const OutputPortBase* pPort);

        /// <summary> Ensure the variable for output port element exists </summary>
        emitters::Variable* EnsureVariableFor(const OutputPortBase* pPort);

        /// <summary> Get the variable for output port element </summary>
        emitters::Variable* GetVariableFor(const PortElementBase& element);

        /// <summary> Ensure the variable for output port element exists </summary>
        emitters::Variable* EnsureVariableFor(const PortElementBase& element);

        /// <summary> Associate the given variable with the output port </summary>
        void SetVariableFor(const OutputPortBase* pPort, emitters::Variable* pVar);

    protected:
        /// <summary>
        /// Create a variable to store computed output for the given output port. The variable
        /// will be emitted lazily.
        /// </summary>
        emitters::Variable* AllocatePortVariable(OutputPortBase* pPort);
        emitters::Variable* GetOrAllocatePortVariable(OutputPortBase* pPort);

        //
        // These methods may be implemented by specific compilers
        //
        virtual void OnBeginCompileNode(const Node& node) {}
        virtual void OnEndCompileNode(const Node& node) {}

    private:
        enum class ArgType
        {
            input,
            output
        };

        void CompileNodes(Model& model);
        const emitters::NamedVariableTypeList& GetArgs() const { return _arguments; }
        const emitters::NamedVariableTypeList& GetInputArgs() const { return _inputArgs; }

        emitters::Variable* AllocArg(emitters::ModuleEmitter& emitter, const OutputPortBase* pPort, ArgType argType);
        void ClearArgs();

        emitters::ModuleEmitter* GetModuleEmitter();

        emitters::NamedVariableTypeList _arguments; // function arguments
        emitters::NamedVariableTypeList _inputArgs; // Track inputs separately

        // A map from output ports to runtime variables
        std::unordered_map<const OutputPortBase*, emitters::Variable*> _portToVarMap;
    };
}
}
