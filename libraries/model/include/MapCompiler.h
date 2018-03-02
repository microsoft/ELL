////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompiler.h (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNodeUtilities.h"
#include "MapCompilerOptions.h"
#include "OutputPort.h"
#include "PortElements.h"

// emitters
#include "CompilerOptions.h"
#include "EmitterTypes.h"
#include "ModuleEmitter.h"
#include "Variable.h"

// stl
#include <stack>
#include <string>
#include <unordered_map>

namespace ell
{
namespace model
{
    class Map;
    class Model;
    class Node;
    
    /// <summary> Abstract base class for ELL model compilers. </summary>
    class MapCompiler
    {
    public:
        virtual ~MapCompiler() = default;

        /// <summary> Compile the map into a function with the given name. </summary>
        ///
        /// <param name="map"> The map to compile. </param>
        /// <param name="functionName"> The name of the function to create. </param>
        /// <param name="parameterNames"> The parameter names of the function to create. </param>
        void CompileMap(Map& map, const std::string& functionName);

        /// <summary> Gets the model-specific compiler parameters being used by the map compiler. </summary>
        ///
        /// <returns> The MapCompilerOptions struct used by the map compiler to control code generation. </returns>
        const MapCompilerOptions& GetMapCompilerOptions() const { return _parameters; }

        //
        // Routines for Node implementers
        //

        /// <summary> Creates a new BlockRegion for the node. </summary>
        ///
        /// <param name="node"> The node we're compiling. </param>
        virtual void NewNodeRegion(const Node& node) = 0;

        /// <summary> Tries to merge the BlockRegion for the current node into the previous block. </summary>
        ///
        /// <param name="node"> The node we're compiling. </param>
        /// <returns> `true` if the regions were merged. </returns>
        virtual bool TryMergeNodeRegion(const Node& node) = 0;

        /// <summary> Tries to merge the BlockRegion for the current node into the BlockRegion from another node. </summary>
        ///
        /// <param name="dest"> The node we merging into. </param>
        /// <param name="src"> The node we're compiling. </param>
        /// <returns> `true` if the regions were merged. </returns>
        virtual bool TryMergeNodeRegions(const Node& dest, const Node& src) = 0;

        /// <summary> Get the variable for output port. </summary>
        emitters::Variable* GetVariableForPort(const OutputPortBase& port);

        /// <summary> Get the variable for output port element. </summary>
        emitters::Variable* GetVariableForElement(const PortElementBase& element);

        /// <summary> Associate the given variable with the output port. </summary>
        void SetVariableForPort(const Port& port, emitters::Variable* pVar);

        /// <summary> Associate the given variable with the output port element. </summary>
        void SetVariableForElement(const PortElementBase& element, emitters::Variable* pVar);

    protected:
        MapCompiler(const MapCompilerOptions& settings);

        /// <summary>
        /// Create a variable to store computed output for the given output port. The variable
        /// will be emitted lazily.
        /// </summary>
        emitters::Variable* AllocatePortVariable(const OutputPortBase& port);
        emitters::Variable* GetOrAllocatePortVariable(const OutputPortBase& port);

        template<typename ValueType>
        emitters::Variable* AllocatePortVariable(const OutputPortBase& port, ValueType initialValue);
        template<typename ValueType>
        emitters::Variable* GetOrAllocatePortVariable(const OutputPortBase& port, ValueType initialValue);

        /// <summary>
        /// Allocate variables for the map function arguments, based on the input and output nodes.
        /// </summary>
        emitters::NamedVariableTypeList AllocateNodeFunctionArguments(Map& map, emitters::ModuleEmitter& emitter);

        //
        // These methods may be implemented by specific compilers
        //
        virtual void OnBeginCompileModel(const Model& model) {}
        virtual void OnEndCompileModel(const Model& model) {}
        virtual void OnBeginCompileNode(const Node& node) {}
        virtual void OnEndCompileNode(const Node& node) {}
        virtual void PushScope();
        virtual void PopScope();
        virtual emitters::ModuleEmitter* GetModuleEmitter() = 0;

    private:
        enum class ArgType
        {
            input,
            output
        };

        friend class CompilableNode;

        void CompileNodes(Model& model);
        emitters::Variable* AllocateNodeFunctionArgument(emitters::ModuleEmitter& emitter, const OutputPortBase* pPort, ArgType argType);
        emitters::Variable* AllocateNodeFunctionArgument(emitters::ModuleEmitter& emitter, const PortElementBase& element, ArgType argType);

        MapCompilerOptions _parameters;
        // map from ports to runtime variables, for all ports in the model
        // stored as a stack, with the top of the stack being the innermost scope
        std::vector<std::unordered_map<const Port*, emitters::Variable*>> _portToVarMaps; // Do we need separate elementToVarMaps?
    };
}
}

#include "../tcc/MapCompiler.tcc"
