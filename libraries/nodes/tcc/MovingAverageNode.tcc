////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MovingAverageNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    MovingAverageNode<ValueType>::MovingAverageNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _windowSize(0)
    {
    }

    template <typename ValueType>
    MovingAverageNode<ValueType>::MovingAverageNode(const model::PortElements<ValueType>& input, size_t windowSize)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = _input.Size();
        for (size_t index = 0; index < _windowSize; ++index)
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
        for (size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index] - lastBufferedSample[index]);
            result[index] = _runningSum[index] / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<MovingAverageNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool MovingAverageNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto delayNode = transformer.AddNode<DelayNode<ValueType>>(newPortElements, _windowSize);
        auto subtractNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newPortElements, delayNode->output, emitters::BinaryOperationType::subtract);
        auto accumNode = transformer.AddNode<AccumulatorNode<ValueType>>(subtractNode->output);
        std::vector<ValueType> literalN(newPortElements.Size(), (ValueType)_windowSize);
        auto constNode = transformer.AddNode<ConstantNode<ValueType>>(literalN);
        auto divideNode = transformer.AddNode<BinaryOperationNode<ValueType>>(accumNode->output, constNode->output, emitters::BinaryOperationType::coordinatewiseDivide);
        transformer.MapNodeOutput(output, divideNode->output);
        return true;
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
        _output.SetSize(dimension);
    }
}
}
