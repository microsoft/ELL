////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cassert>

namespace nodes
{
    template <typename ValueType>
    LinearNode<ValueType>::LinearNode(const model::OutputPortElementList<ValueType>& input, const std::vector<ValueType>& weights, ValueType bias) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1), _input2(this, input2), _output(this, 1), _weights(weights), _bias(bias)
    {
        assert(input1.Size() == input2.Size());
        assert(weights.Size() == input.Size());
    }

    template <typename ValueType>
    std::vector<ValueType> LinearNode<ValueType>::Compute() const
    {
        auto result = _bias;
        for (size_t index = 0; index < _input.Size(); index++)
        {
            result += _input[index] * _weights[index];
        }
        _output.SetOutput({ result });
    }

    template <typename ValueType>
    void LinearNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput1 = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<LinearNode<ValueType>>(newInput, _weights, _bias);
        transformer.MapOutputPort(output, newNode->output);
    }
}
