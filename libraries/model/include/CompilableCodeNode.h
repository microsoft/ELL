////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableCodeNode.h (model)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"

#include <value/include/FunctionDeclaration.h>
#include <value/include/Value.h>

#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class CompilableCodeNode : public CompilableNode
    {
    public:
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "CompilableCodeNode"; }

    protected:
        CompilableCodeNode(std::string name, const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> This is where you implement your code node function </summary>
        virtual void Define(value::FunctionDeclaration& fn) = 0;

        /// <summary> You can optionally also define a function that will be called to reset any node state.
        /// For example this method could clear the value of one or more StaticAllocated variables defined by the
        /// Define method above. </summary>
        virtual void DefineReset(value::FunctionDeclaration& fn) {}

        /// <summary> Return a name to be used in serialized model.  You shouldn't need to override this method
        /// unless you want to return a name that is backwards compatible with old node names </summary>
        std::string GetRuntimeTypeName() const override;

    private:
        std::string GetCompiledFunctionName() const final;

        void Reset() final;

        bool HasOwnFunction() const final;

        void EmitNodeFunction(IRMapCompiler& compiler) final;

        void Compute() const final;

        void SetFunctionParameters() const;

        std::string _name;
        mutable value::FunctionDeclaration _fn;
        mutable value::FunctionDeclaration _resetFn;
    };
} // namespace model
} // namespace ell
