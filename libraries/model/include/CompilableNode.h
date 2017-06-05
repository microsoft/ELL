////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"

// emitters
#include "EmitterTypes.h"
#include "IRFunctionEmitter.h"

// utilities
#include "TypeName.h"

// llvm
#include "llvm/IR/Value.h"

// stl
#include <string>

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
        void CompileNode(MapCompiler& compiler); // TODO: call this something different from the virtual Compile functions. Call them "CompileContents" or something

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const { return true; }

    protected:
        CompilableNode(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs)
            : Node(inputs, outputs) {}
        virtual ~CompilableNode() = default;

        // Compile function necessary to override in subclasses
        virtual void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) = 0;

        virtual bool ShouldCompileInline() const;
        virtual std::string GetCompiledFunctionName() const;
        virtual bool HasState() const; // TODO: add to Node, it'll be useful even for non-compilable nodes
        virtual std::string GetStateIdentifier() const;

        virtual void DeclareNodeFunction(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction); // #### never used
        virtual void CallNodeFunction(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction);
        virtual emitters::NamedVariableTypeList GetNodeFunctionParameterList(IRMapCompiler& compiler) const;
        virtual std::vector<llvm::Value*> GetNodeFunctionArguments(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const;
    };
}
}
