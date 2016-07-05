////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DelayNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DelayNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    template <typename ValueType>
    DelayNode<ValueType>::DelayNode(const model::OutputPortElementList<ValueType>& input, size_t windowSize) : Node({&_input}, {&_output}), _input(this, input), _output(this, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for(size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compute() const
    {
        auto lastBufferedSample = _samples[0];        
        _samples.push_back(_input.GetValue());
        _samples.erase(_samples.begin());
        _output.SetOutput(lastBufferedSample);
    };

    template <typename ValueType>
    void DelayNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }
}
