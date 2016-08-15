////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode() : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _operation(OperationType::none)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, OperationType operation) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _operation(operation)
    {
        if (input1.Size() != input2.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }
        assert(input1.Size() == input2.Size());
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> BinaryOperationNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<ValueType>(_input1.Size());
        for (size_t index = 0; index < _input1.Size(); index++)
        {
            output[index] = fn(_input1[index], _input2[index]);
        }
        return output;
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
            case OperationType::add:
                output = ComputeOutput([](ValueType x, ValueType y) { return x + y; });
                break;
            case OperationType::subtract:
                output = ComputeOutput([](ValueType x, ValueType y) { return x - y; });
                break;
            case OperationType::coordinatewiseMultiply:
                output = ComputeOutput([](ValueType x, ValueType y) { return x * y; });
                break;
            case OperationType::divide:
                output = ComputeOutput([](ValueType x, ValueType y) { return x / y; });
                break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryOperationNode<ValueType>>(PortElements1, PortElements2, _operation);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("operation", static_cast<int>(_operation));
        serializer.Serialize("input1", _input1);
        serializer.Serialize("input2", _input2);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        int op = 0;
        serializer.Deserialize("operation", op, context);
        _operation = static_cast<OperationType>(op);
        serializer.Deserialize("input1", _input1, context);
        serializer.Deserialize("input2", _input2, context);
    }
}
