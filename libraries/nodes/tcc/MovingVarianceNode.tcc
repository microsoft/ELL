////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MovingVarianceNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    MovingVarianceNode<ValueType>::MovingVarianceNode(const model::OutputPortElements<ValueType>& input, size_t windowSize) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for (size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
        _runningSquaredSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Compute() const
    {
        static auto squared = [](const ValueType& x) { return x * x; };

        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for (size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index] - lastBufferedSample[index]);
            _runningSquaredSum[index] += squared(inputSample[index]) - squared(lastBufferedSample[index]);
            result[index] = (_runningSquaredSum[index] - (squared(_runningSum[index]) / _windowSize)) / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MovingVarianceNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MovingVarianceNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }
}
