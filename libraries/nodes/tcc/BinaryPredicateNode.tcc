////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryPredicateNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    namespace BinaryPredicates
    {
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
    BinaryPredicateNode<ValueType>::BinaryPredicateNode() : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _predicate(PredicateType::none)
    {}

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
                output = ComputeOutput(BinaryPredicates::Equal<ValueType>);
                break;
            case PredicateType::less:
                output = ComputeOutput(BinaryPredicates::Less<ValueType>);
                break;
            case PredicateType::greater:
                output = ComputeOutput(BinaryPredicates::Greater<ValueType>);
                break;
            case PredicateType::notEqual:
                output = ComputeOutput(BinaryPredicates::NotEqual<ValueType>);
                break;
            case PredicateType::lessOrEqual:
                output = ComputeOutput(BinaryPredicates::LessOrEqual<ValueType>);
                break;
            case PredicateType::greaterOrEqual:
                output = ComputeOutput(BinaryPredicates::GreaterOrEqual<ValueType>);
                break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown predicate type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    utilities::ObjectDescription BinaryPredicateNode<ValueType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, BinaryPredicateNode<ValueType>>("Binary predicate node");
        description.template AddProperty<decltype(_input1)>(input1PortName, "Input port 1");
        description.template AddProperty<decltype(_input2)>(input2PortName, "Input port 2");
        description.template AddProperty<decltype(_output)>(outputPortName, "Output port");
        description.template AddProperty<int>("predicate", "Predicate code");
        return description;
    }

    template <typename ValueType>
    utilities::ObjectDescription BinaryPredicateNode<ValueType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, BinaryPredicateNode<ValueType>>();
        description[input1PortName] = _input1;
        description[input2PortName] = _input2;
        description[outputPortName] = _output;
        description["predicate"] = static_cast<int>(_predicate);
        return description;
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description[input1PortName] >> _input1;
        description[input2PortName] >> _input2;
        description[outputPortName] >> _output;
        int predicate = 0;
        description["predicate"] >> predicate;
        _predicate = static_cast<PredicateType>(predicate);
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
