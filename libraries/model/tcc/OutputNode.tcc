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
        : OutputNodeBase(_input, _output, {}), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0) 
    {
        SetShape({});
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input)
        : OutputNodeBase(_input, _output, MemoryShape{ static_cast<int>(input.Size()) }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(MemoryShape{ static_cast<int>(input.Size()) });
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input, const MemoryShape& shape)
        : OutputNodeBase(_input, _output, shape), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(shape);
    }

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
        archiver["layout"] << _input.GetMemoryLayout();
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;

        int size;
        archiver.OptionalProperty("size", 0) >> size;
        std::vector<int> shapeVector;
        archiver.OptionalProperty("shape", std::vector<int>{size}) >> shapeVector;
        if (archiver.HasNextPropertyName("layout"))
        {
            PortMemoryLayout layout;
            archiver["layout"] >> layout;
            SetShape(layout.GetActiveSize());
        }
        else
        {
            SetShape({shapeVector});
        }
    }
}
}
