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
    MovingAverageNode<ValueType>::MovingAverageNode() : Node({&_input}, {&_output}), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _windowSize(0)
    {        
    }

    template <typename ValueType>
    MovingAverageNode<ValueType>::MovingAverageNode(const model::OutputPortElements<ValueType>& input, size_t windowSize) : Node({&_input}, {&_output}), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = _input.Size();
        for(size_t index = 0; index < _windowSize; ++index)
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
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<MovingAverageNode<ValueType>>(newOutputPortElements, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto delayNode = transformer.AddNode<DelayNode<ValueType>>(newOutputPortElements, _windowSize);
        auto subtractNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newOutputPortElements, delayNode->output, BinaryOperationNode<ValueType>::OperationType::subtract);
        auto accumNode = transformer.AddNode<AccumulatorNode<ValueType>>(subtractNode->output);
        std::vector<ValueType> literalN(newOutputPortElements.Size(), (ValueType)_windowSize);
        auto constNode = transformer.AddNode<ConstantNode<ValueType>>(literalN);
        auto divideNode = transformer.AddNode<BinaryOperationNode<ValueType>>(accumNode->output, constNode->output, BinaryOperationNode<ValueType>::OperationType::divide);
        transformer.MapOutputPort(output, divideNode->output);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
        serializer.Serialize("windowSize", _windowSize);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("windowSize", _windowSize, context);

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for(size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
    }
}
