////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode() : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _operation(OperationType::none)
    {
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::PortElements<ValueType>& input, OperationType operation) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _operation(operation)
    {
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> UnaryOperationNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<ValueType>(_input.Size());
        for (size_t index = 0; index < _input.Size(); index++)
        {
            output[index] = fn(_input[index]);
        }
        return output;
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
            case OperationType::sqrt:
            {
                auto sqrtOp = [](ValueType x) { return std::sqrt(x); };
                output = ComputeOutput(sqrtOp);
            }
            break;
            
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newPortElements, _operation);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("operation", static_cast<int>(_operation));
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        int op = 0;
        serializer.Deserialize("operation", op, context);
        _operation = static_cast<OperationType>(op);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
    }
}
