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
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::OutputPortElements<ValueType>& input1, const model::OutputPortElements<ValueType>& input2, OperationType operation) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _operation(operation)
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
        auto OutputPortElements1 = transformer.TransformOutputPortElements(_input1.GetOutputPortElements());
        auto OutputPortElements2 = transformer.TransformOutputPortElements(_input2.GetOutputPortElements());
        auto newNode = transformer.AddNode<BinaryOperationNode<ValueType>>(OutputPortElements1, OutputPortElements2, _operation);
        transformer.MapOutputPort(output, newNode->output);
    }

    /// Inherited from ISerializable
    // template <typename ValueType>
    // utilities::ObjectDescription BinaryOperationNode<ValueType>::GetDescription() const
    // {
    //     std::cout << "Serializing BinaryOperationNode" << std::endl;
    //     auto result = Node::GetDescription();
    //     result.AddField("operation", (int)_operation);
    //     return result;
    // }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        std::cout << "Serializing BinaryOperationNode" << std::endl;
        Node::Serialize(serializer);
        serializer.Serialize("operation", (int)_operation);
        serializer.Serialize("operation", _input1);
        serializer.Serialize("operation", _input2);
    }
}
