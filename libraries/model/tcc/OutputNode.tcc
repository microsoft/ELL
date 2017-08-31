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
        : OutputNodeBase(_input, _output, OutputShape{ 0,0,0 }), _input(this, {}, inputPortName), _output(this, outputPortName, 0) {};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input)
        : OutputNodeBase(_input, _output, OutputShape{ input.Size(), 1, 1 }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()) {};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input, OutputShape shape)
        : OutputNodeBase(_input, _output, shape), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()) {};

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
        archiver[inputPortName] << _input;
        ell::math::TensorShape shape = GetShape();
        archiver[shapeName] << std::vector<size_t>({ shape.rows,shape.columns,shape.channels });
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        std::vector<size_t> shapeVector;
        archiver[shapeName] >> shapeVector;
        _output.SetSize(_input.Size());
        if (shapeVector.size() >= 3) {
            OutputShape shape{ shapeVector[0], shapeVector[1], shapeVector[2] };
            SetShape(shape);
        }
    }
}
}
