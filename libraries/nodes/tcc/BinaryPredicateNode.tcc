////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryPredicateNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ADD_TO_STRING_ENTRY(NAMESPACE, OPERATOR) \
    case NAMESPACE::OPERATOR:                    \
        return #OPERATOR;
#define BEGIN_FROM_STRING if (false)
#define ADD_FROM_STRING_ENTRY(NAMESPACE, OPERATOR) else if (name == #OPERATOR) return NAMESPACE::OPERATOR

namespace ell
{
namespace nodes
{
    namespace BinaryPredicates
    {
        inline std::string to_string(BinaryPredicateType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(BinaryPredicateType, none);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, equal);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, less);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, greater);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, notEqual);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, lessOrEqual);
                ADD_TO_STRING_ENTRY(BinaryPredicateType, greaterOrEqual);
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary predicate");
            }
        }

        inline BinaryPredicateType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, none);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, equal);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, less);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, greater);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, notEqual);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, lessOrEqual);
            ADD_FROM_STRING_ENTRY(BinaryPredicateType, greaterOrEqual);

            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary predicate");
        }

        template <typename ValueType>
        bool Equal(ValueType a, ValueType b)
        {
            return a == b;
        }

        template <typename ValueType>
        bool Less(ValueType a, ValueType b)
        {
            return a < b;
        }

        template <typename ValueType>
        bool Greater(ValueType a, ValueType b)
        {
            return a > b;
        }

        template <typename ValueType>
        bool NotEqual(ValueType a, ValueType b)
        {
            return a != b;
        }

        template <typename ValueType>
        bool LessOrEqual(ValueType a, ValueType b)
        {
            return a <= b;
        }

        template <typename ValueType>
        bool GreaterOrEqual(ValueType a, ValueType b)
        {
            return a >= b;
        }
    }

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode()
        : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _predicate(BinaryPredicateType::none)
    {
    }

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, BinaryPredicateType predicate)
        : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _predicate(predicate)
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
            case BinaryPredicateType::equal:
                output = ComputeOutput(BinaryPredicates::Equal<ValueType>);
                break;
            case BinaryPredicateType::less:
                output = ComputeOutput(BinaryPredicates::Less<ValueType>);
                break;
            case BinaryPredicateType::greater:
                output = ComputeOutput(BinaryPredicates::Greater<ValueType>);
                break;
            case BinaryPredicateType::notEqual:
                output = ComputeOutput(BinaryPredicates::NotEqual<ValueType>);
                break;
            case BinaryPredicateType::lessOrEqual:
                output = ComputeOutput(BinaryPredicates::LessOrEqual<ValueType>);
                break;
            case BinaryPredicateType::greaterOrEqual:
                output = ComputeOutput(BinaryPredicates::GreaterOrEqual<ValueType>);
                break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown predicate type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[input1PortName] << _input1;
        archiver[input2PortName] << _input2;
        archiver[outputPortName] << _output;
        archiver["predicate"] << BinaryPredicates::to_string(_predicate);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[input1PortName] >> _input1;
        archiver[input2PortName] >> _input2;
        archiver[outputPortName] >> _output;
        std::string predicate;
        archiver["predicate"] >> predicate;
        _predicate = BinaryPredicates::from_string(predicate);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryPredicateNode<ValueType>>(PortElements1, PortElements2, _predicate);
        transformer.MapNodeOutput(output, newNode->output);
    }
}
}
