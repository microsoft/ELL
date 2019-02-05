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

        virtual void Define(value::FunctionDeclaration& fn) = 0;

    private:
        std::string GetRuntimeTypeName() const final;

        std::string GetCompiledFunctionName() const final;

        bool HasOwnFunction() const final;

        void EmitNodeFunction(IRMapCompiler& compiler) final;

        void Compute() const final;

        void SetFunctionParameters() const;

        std::string _name;
        mutable value::FunctionDeclaration _fn;
    };
} // namespace model
} // namespace ell
