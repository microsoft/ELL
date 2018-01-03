////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ValueType>
    OutputNode<ValueType>::OutputNode()
        : OutputNodeBase(_input, _output, OutputShape{ 0, 0, 0 }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0) {};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input)
        : OutputNodeBase(_input, _output, OutputShape{ input.Size(), 1, 1 }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size()) {};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input, const OutputShape& shape)
        : OutputNodeBase(_input, _output, shape), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size()) {};

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newPortElements, GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[shapeName] << static_cast<std::vector<size_t>>(GetShape());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::vector<size_t> shapeVector; 
        archiver[shapeName] >> shapeVector;
        _output.SetSize(_input.Size());
        if (shapeVector.size() >= 3) 
        {
            SetShape(OutputShape{ shapeVector });
        }
    }
}
}
