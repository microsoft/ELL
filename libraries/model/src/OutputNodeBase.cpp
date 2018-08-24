////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNodeBase.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNodeBase.h"
#include "IRMapCompiler.h"
#include "OutputNodeBase.h"

namespace ell
{
namespace model
{
    OutputNodeBase::OutputNodeBase(InputPortBase& input, OutputPortBase& output, const MemoryShape& shape)
        : CompilableNode({ &input }, { &output }), _inputBase(input), _outputBase(output)
    {
    }

    OutputNodeBase::OutputNodeBase(const std::vector<InputPortBase*>& inputs, OutputPortBase& output, const MemoryShape& shape)
        : CompilableNode(inputs, { &output }), _inputBase(*inputs.at(0)), _outputBase(output)
    {
    }

    MemoryShape OutputNodeBase::GetShape() const 
    { 
        return _outputBase.GetMemoryLayout().GetActiveSize();
    }

    void OutputNodeBase::SetShape(const MemoryShape& shape) 
    { 
        _outputBase.SetMemoryLayout({shape});
    } 

    ell::utilities::ArchiveVersion OutputNodeBase::GetArchiveVersion() const
    {
        return {ell::utilities::ArchiveVersionNumbers::v2};
    }

    void OutputNodeBase::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto inputType = _inputBase.GetType();
        auto outputType = _outputBase.GetType();
        if (inputType != outputType)
        {
            using namespace std::string_literals;
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "OutputNode: input type "s + GetPortCTypeName(inputType) + " doesn't match output type " + GetPortCTypeName(outputType));
        }

        if (!_inputBase.IsValid())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Compiling invalid input port");
        }

        auto output = function.LocalArray(compiler.EnsurePortEmitted(_outputBase));
        // check if the output variable is null.
        function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [output, &compiler, this](emitters::IRFunctionEmitter& function) {
            auto input = function.LocalArray(compiler.EnsurePortEmitted(_inputBase));
            auto size = _inputBase.Size();
            function.For(size, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                output[i] = input[i];
            });
        });
    }
}
}
