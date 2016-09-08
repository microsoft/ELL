////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DelayNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    DelayNode<ValueType>::DelayNode(const model::PortElements<ValueType>& input, size_t windowSize)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for (size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }

    template <typename ValueType>
    DelayNode<ValueType>::DelayNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _windowSize(0)
    {
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void DelayNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        archiver["windowSize"] >> _windowSize;

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }
}
