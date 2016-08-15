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
    BinaryPredicateNode<ValueType>::BinaryPredicateNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, PredicateType predicate) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _predicate(predicate)
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
    void BinaryPredicateNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryPredicateNode<ValueType>>(PortElements1, PortElements2, _predicate);
        transformer.MapOutputPort(output, newNode->output);
    }
}
