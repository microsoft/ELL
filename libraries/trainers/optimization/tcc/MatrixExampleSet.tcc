////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixExampleSet.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename ElementType>
    MatrixExampleSet<ElementType>::MatrixExampleSet(math::RowMatrix<ElementType> input, math::RowMatrix<ElementType> output) : 
        _input(std::move(input)),
        _output(std::move(output))
    {
        if (_input.NumRows() != _output.NumRows())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidSize, "Number of inputs and outputs don't match");
        }
    }

    template <typename ElementType>
    auto MatrixExampleSet<ElementType>::Get(size_t index) const -> ExampleType
    {
        return ExampleType(_input.GetRow(index), _output.GetRow(index));
    }
}
}
}