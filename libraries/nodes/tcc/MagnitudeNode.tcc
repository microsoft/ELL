////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MagnitudeNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MagnitudeNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cmath>

namespace nodes
{
    template <typename ValueType>
    MagnitudeNode<ValueType>::MagnitudeNode(const model::OutputPortElementList<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input), _output(this, 1)
    {
    }

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for(size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v*v);
        }
        _output.SetOutput({std::sqrt(result)});
    };

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MagnitudeNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // TODO: elementwise x^2, sum, div by D
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MagnitudeNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }
}
