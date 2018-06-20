////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.cpp (model)
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
    OutputNodeBase::OutputNodeBase(InputPortBase& input, OutputPortBase& output, const math::TensorShape& shape)
        : CompilableNode({ &input }, { &output }), _inputBase(input), _outputBase(output), _shape(shape)
    {
    }

    OutputNodeBase::OutputNodeBase(const std::vector<InputPortBase*>& inputs, OutputPortBase& output, const math::TensorShape& shape)
        : CompilableNode(inputs, { &output }), _inputBase(*inputs.at(0)), _outputBase(output), _shape(shape)
    {
    }

    ell::utilities::ArchiveVersion OutputNodeBase::GetArchiveVersion() const
    {
        return ell::utilities::ArchiveVersion{ 2 };
    }

    void OutputNodeBase::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(GetPortVariableType(_inputBase) == GetPortVariableType(_outputBase));

        auto inputIsInputNode = (dynamic_cast<const InputNodeBase*>(_inputBase.GetInputElement(0).ReferencedPort()->GetNode()) != nullptr);
        // TODO: re-enable this branch when scalar port bug is fixed 
        if (IsPureVector(_inputBase) && _inputBase.Size() != 1 && _outputBase.Size() != 1 && !inputIsInputNode && false)
        {
            auto pVar = compiler.GetVariableForElement(_inputBase.GetInputElement(0));
            compiler.SetVariableForPort(_outputBase, pVar);
        }
        else
        {
            auto output = function.LocalArray(compiler.EnsurePortEmitted(_outputBase));
            // check if the output variable is null.
            function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [output, &compiler, this](emitters::IRFunctionEmitter& function) {
                if (_inputBase.Size() == 1)
                {
                    llvm::Value* pVal = compiler.LoadPortElementVariable(_inputBase.GetInputElement(0));
                    function.Store(output, pVal);
                }
                else
                {
                    auto inputElements = _inputBase.GetInputElements();
                    int rangeStart = 0;
                    for (auto range : inputElements.GetRanges())
                    {
                        auto input = function.LocalArray(compiler.EnsurePortEmitted(*range.ReferencedPort()));
                        auto rangeSize = range.Size();
                        function.For(rangeSize, [range, rangeStart, input, output](emitters::IRFunctionEmitter& function, auto i) {
                            auto inputIndex = i + static_cast<int>(range.GetStartIndex());
                            auto outputIndex = i + rangeStart;
                            output[outputIndex] = input[inputIndex];
                        });
                        rangeStart += rangeSize;
                    }
                }
            });
        }
    }

}
}
