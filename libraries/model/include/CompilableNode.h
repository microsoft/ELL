////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRModelProfiler.h"
#include "Node.h"
#include "IRModelProfiler.h"

// emitters
#include "EmitterTypes.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"

// utilities
#include "TypeName.h"

// llvm
#include <llvm/IR/Value.h>

// stl
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class MapCompiler;
    class IRMapCompiler;

    class CompilableNode : public Node
    {
    public:
        /// <summary> Compile the node with the given compiler </summary>
        ///
        /// <param name="compiler"> The compiler to use when compiling the node </param>
        void CompileNode(MapCompiler& compiler);

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const MapCompiler* compiler) const override { return true; }

    protected:
        CompilableNode(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs)
            : Node(inputs, outputs) {}
        ~CompilableNode() override = default;

        // Compile function necessary to override in subclasses
        virtual void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        //
        // Virtual functions to optionally change how the node is compiled
        //

        // Returns true if the compiler should try to inline the node. The default implementation is
        // a heuristic based on the complexity of the input ports.
        // Subclasses should override if they need different behavior.
        virtual bool ShouldCompileInline() const;

        // Returns the name of the function the node is compiled into.
        virtual std::string GetCompiledFunctionName() const;

        // Returns false by default. Subclasses can return true to if they create their own function instead
        // of emitting inline IR.
        virtual bool HasOwnFunction() const;

        // If `HasOwnFunction` returns true, this function must emit a function into the module.
        // The default implementation throws a `notImplemented` exception.
        virtual void EmitNodeFunction(emitters::IRModuleEmitter& module);

        // Subclasses can override this function to indicate that they can return LLVM IR as their node function implementation.
        virtual bool HasPrecompiledIR() const;

        // If `HasPrecompiledIR` returns true, this function must return the LLVM IR for the function implementing the node.
        virtual std::string GetPrecompiledIR() const;

        // Returns an identifier that uniquely identifies the internal state that is implicitly compiled into the node function.
        // The default implementation returns nothing (the empty string) for nodes with no state (`HasState` returns false),
        // and a unqiue identifier (essentially, the Node's ID) for nodes with data (`HasState` returns true). This implies that each
        // node with internal state gets its code compiled int a separate function.
        //
        // Subclasses can override this if there is some state that gets compiled into the node but is shared among different node instances.
        virtual std::string GetInternalStateIdentifier() const;

        // Returns a list of additional "state" parameters (beyond the input and output ports) that should be passed to the node's compute function.
        // The default implementation returns an empty list, as by default, node's don't have any extra state parameters.
        // Subclasses must override this if they want to pass external state into the function.
        virtual emitters::NamedVariableTypeList GetNodeFunctionStateParameterList(IRMapCompiler& compiler) const;

        // Returns the list of actual arguments for the "state" parameters to the node's compute function,.
        // The default implementation returns an empty list, as by default, node's don't have any extra state parameters.
        // Subclasses must override this if they override `GetNodeFunctionStateParameterList`
        virtual std::vector<llvm::Value*> GetNodeFunctionStateArguments(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const;

        // Returns the list of formal parameters for the node's compute function. By default, the parameters include the input ports,
        // any additional "state" parameters, followed by the output ports.
        // Subclasses may override this method if they want to modify the node's function signature differently.
        virtual emitters::NamedVariableTypeList GetNodeFunctionParameterList(IRMapCompiler& compiler) const;

        // Returns the list of actual argument values to use when calling the node's compute function. By default, the arguments include the
        // values for the input ports, any special "state" parameters, and the values for the output ports.
        // Subclasses must override this if they override `GetNodeFunctionParameterList`
        virtual std::vector<llvm::Value*> GetNodeFunctionArguments(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const;

        // Emits code for calling the function that implements this node.
        // The default implementation uses the result of `GetCompiledFunctionName` and `GetNodeFunctionParameterList`
        // Sublasses shouldn't have to override this method, but they can if they need to fully customize the call site
        // for the node's compute function.
        virtual void CallNodeFunction(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction);

    private:
        const std::string _nodeFunctionPrefix = "_Node__";
        const char _badIdentifierChars[3] = {'<', '>', ','};
    };
}
}
