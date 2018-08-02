////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ValueType>
    InputNode<ValueType>::InputNode()
        : InputNodeBase(_output), _output(this, defaultOutputPortName, 0)
    {
        SetShape(MemoryShape{ 0 });
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(size_t size)
        : InputNodeBase(_output), _output(this, defaultOutputPortName, size)
    {
        SetShape(MemoryShape{ static_cast<int>(size) });
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(MemoryShape shape)
        : InputNodeBase(_output), _output(this, defaultOutputPortName, PortMemoryLayout{ shape })
    {
        SetShape(shape);
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(const PortMemoryLayout& layout)
        : InputNodeBase(_output), _output(this, defaultOutputPortName, layout)
    {
        SetMemoryLayout(layout);
    }

    template <typename ValueType>
    void InputNode<ValueType>::SetInput(ValueType inputValue)
    {
        SetInput(std::vector<ValueType>{ inputValue });
    }

    template <typename ValueType>
    void InputNode<ValueType>::SetInput(std::vector<ValueType> inputValues)
    {
        assert(_output.Size() == inputValues.size());
        _inputValues = inputValues;
    }

    template <typename ValueType>
    void InputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_inputValues);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Input node is typically set up during pass1. By default, no further work needed
    }

    template <typename ValueType>
    void InputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver["layout"] << _output.GetMemoryLayout();
    }

    template <typename ValueType>
    void InputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

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
