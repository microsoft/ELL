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
    MovingVarianceNode<ValueType>::MovingVarianceNode() : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _windowSize(0)
    {
    }
        
    template <typename ValueType>
    MovingVarianceNode<ValueType>::MovingVarianceNode(const model::PortElements<ValueType>& input, size_t windowSize) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = _input.Size();
        for (size_t index = 0; index < _windowSize; ++index)
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<MovingVarianceNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
        serializer.Serialize("windowSize", _windowSize);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("windowSize", _windowSize, context);

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
        _runningSquaredSum = std::vector<ValueType>(dimension);
    }
}
