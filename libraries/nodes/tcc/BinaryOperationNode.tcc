////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    namespace BinaryOperations
    {
        template <typename ValueType>
        ValueType Add(ValueType a, ValueType b)
        {
            return a+b;
        }

        template <>
        inline bool Add(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Subtract(ValueType a, ValueType b)
        {
            return a-b;
        }

        template <>
        inline bool Subtract(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Multiply(ValueType a, ValueType b)
        {
            return a*b;
        }

        template <>
        inline bool Multiply(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Divide(ValueType a, ValueType b)
        {
            return a/b;
        }

        template <>
        inline bool Divide(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        //
        // Logical operations
        // 
        template <typename ValueType>
        ValueType LogicalAnd(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalAnd(bool a, bool b)
        {
            return a&&b;
        }

        template <typename ValueType>
        ValueType LogicalOr(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalOr(bool a, bool b)
        {
            return a || b;
        }

        template <typename ValueType>
        ValueType LogicalXor(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalXor(bool a, bool b)
        {
            return (!a) != (!b);
        }
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode() : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _operation(OperationType::none)
    {}

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
    std::vector<ValueType> BinaryOperationNode<ValueType>::ComputeOutput(Operation&& function) const
    {
        auto output = std::vector<ValueType>(_input1.Size());
        for (size_t index = 0; index < _input1.Size(); index++)
        {
            output[index] = function(_input1[index], _input2[index]);
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
                output = ComputeOutput(BinaryOperations::Add<ValueType>);
                break;
            case OperationType::subtract:
                output = ComputeOutput(BinaryOperations::Subtract<ValueType>);
                break;
            case OperationType::coordinatewiseMultiply:
                output = ComputeOutput(BinaryOperations::Multiply<ValueType>);
                break;
            case OperationType::divide:
                output = ComputeOutput(BinaryOperations::Divide<ValueType>);
                break;
            case OperationType::logicalAnd:
                output = ComputeOutput(BinaryOperations::LogicalAnd<ValueType>);
                break;
            case OperationType::logicalOr:
                output = ComputeOutput(BinaryOperations::LogicalOr<ValueType>);
                break;
            case OperationType::logicalXor:
                output = ComputeOutput(BinaryOperations::LogicalXor<ValueType>);
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
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::AddProperties(utilities::Archiver& archiver) const
    {
        Node::AddProperties(archiver);
        archiver[input1PortName] << _input1;
        archiver[input2PortName] << _input2;
        archiver[outputPortName] << _output;
        archiver["operation"] << static_cast<int>(_operation);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        Node::SetObjectState(archiver, context);
        archiver[input1PortName] >> _input1;
        archiver[input2PortName] >> _input2;
        archiver[outputPortName] >> _output;
        int operation = 0;
        archiver["operation"] >> operation;
        _operation = static_cast<OperationType>(operation);
    }
}
