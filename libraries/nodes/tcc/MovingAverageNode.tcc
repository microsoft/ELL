////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MovingAverageNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    MovingAverageNode<ValueType>::MovingAverageNode(const model::OutputPortElementList<ValueType>& input, size_t windowSize) : Node({&_input}, {&_output}), _input(this, input), _output(this, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for(size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Compute() const
    {
        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for(size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index]-lastBufferedSample[index]);
            result[index] = _runningSum[index] / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MovingAverageNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MovingAverageNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }
}
