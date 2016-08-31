////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    namespace UnaryOperations
    {
        template <typename ValueType>
        ValueType Sqrt(ValueType a)
        {
            return std::sqrt(a);
        }

        template <>
        inline bool Sqrt(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking sqrt of a boolean value");
        }

        template <typename ValueType>
        ValueType LogicalNot(ValueType a)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking not of a non-boolean value");
        }

        template <>
        inline bool LogicalNot(bool x)
        {
            return !x;
        }
    }


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
    std::vector<ValueType> UnaryOperationNode<ValueType>::ComputeOutput(Operation&& function) const
    {
        auto output = std::vector<ValueType>(_input.Size());
        for (size_t index = 0; index < _input.Size(); index++)
        {
            output[index] = function(_input[index]);
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
                output = ComputeOutput(UnaryOperations::Sqrt<ValueType>);
            }
            break;
            case OperationType::logicalNot:
            {
                output = ComputeOutput(UnaryOperations::LogicalNot<ValueType>);
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
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    utilities::ObjectDescription UnaryOperationNode<ValueType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, UnaryOperationNode<ValueType>>("Accumulator node");
        description.template AddProperty<decltype(_input)>(inputPortName, "Input port");
        description.template AddProperty<decltype(_output)>(outputPortName, "Output port");
        description.template AddProperty<int>("operation", "Operation code");
        return description;
    }

    template <typename ValueType>
    utilities::ObjectDescription UnaryOperationNode<ValueType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, UnaryOperationNode<ValueType>>();
        description[inputPortName] = _input;
        description[outputPortName] = _output;
        description["operation"] = static_cast<int>(_operation);
        return description;
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description[inputPortName] >> _input;
        description[outputPortName] >> _output;
        int operation = 0;
        description["operation"] >> operation;
        _operation = static_cast<OperationType>(operation);
    }
}
