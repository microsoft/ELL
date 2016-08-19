////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryPredicateNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode() : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _predicate(PredicateType::none)
    {}

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode(const model::OutputPortElements<ValueType>& input1, const model::OutputPortElements<ValueType>& input2, PredicateType predicate) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _predicate(predicate)
    {
        if (input1.Size() != input2.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }
        assert(input1.Size() == input2.Size());
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<bool> BinaryPredicateNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<bool>(_input1.Size());
        for (size_t index = 0; index < _input1.Size(); index++)
        {
            output[index] = fn(_input1[index], _input2[index]);
        }
        return output;
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Compute() const
    {
        std::vector<bool> output;
        switch (_predicate)
        {
            case PredicateType::equal:
                output = ComputeOutput([](ValueType x, ValueType y) { return x == y; });
                break;
            case PredicateType::less:
                output = ComputeOutput([](ValueType x, ValueType y) { return x < y; });
                break;
            case PredicateType::greater:
                output = ComputeOutput([](ValueType x, ValueType y) { return x > y; });
                break;
            case PredicateType::notEqual:
                output = ComputeOutput([](ValueType x, ValueType y) { return x != y; });
                break;
            case PredicateType::lessOrEqual:
                output = ComputeOutput([](ValueType x, ValueType y) { return x <= y; });
                break;
            case PredicateType::greaterOrEqual:
                output = ComputeOutput([](ValueType x, ValueType y) { return x >= y; });
                break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown predicate type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("predicate", static_cast<int>(_predicate));
        serializer.Serialize("input1", _input1);
        serializer.Serialize("input2", _input2);
        serializer.Serialize("output", _output);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        int pred = 0;
        serializer.Deserialize("predicate", pred, context);
        _predicate = static_cast<PredicateType>(pred);
        serializer.Deserialize("input1", _input1, context);
        serializer.Deserialize("input2", _input2, context);
        serializer.Deserialize("output", _output, context);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto OutputPortElements1 = transformer.TransformOutputPortElements(_input1.GetOutputPortElements());
        auto OutputPortElements2 = transformer.TransformOutputPortElements(_input2.GetOutputPortElements());
        auto newNode = transformer.AddNode<BinaryPredicateNode<ValueType>>(OutputPortElements1, OutputPortElements2, _predicate);
        transformer.MapOutputPort(output, newNode->output);
    }
}
