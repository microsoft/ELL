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
        : InputNodeBase(_output, InputShape{ 0,0,0 }), _output(this, defaultOutputPortName, 0) {};

    template <typename ValueType>
    InputNode<ValueType>::InputNode(size_t size)
        : InputNodeBase(_output, InputShape{ size,1,1 }), _output(this, defaultOutputPortName, size) {};


    template <typename ValueType>
    InputNode<ValueType>::InputNode(InputShape shape)
        : InputNodeBase(_output, shape), _output(this, defaultOutputPortName, shape.Size()) {};

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
        archiver["size"] << _output.Size();
        InputShape shape = GetShape();
        archiver["shape"] << std::vector<size_t>({ shape.NumRows(),shape.NumColumns(),shape.NumChannels() });
    }

    template <typename ValueType>
    void InputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        int size;
        archiver["size"] >> size;
        std::vector<size_t> shapeVector;
        archiver["shape"] >> shapeVector;
        _output.SetSize(size);
        if (shapeVector.size() >= 3) {
            InputShape shape{ shapeVector[0], shapeVector[1], shapeVector[2] };
            SetShape(shape);
        }
    }
}
}
