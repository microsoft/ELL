////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
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
    namespace BinaryOperations
    {
        inline std::string to_string(BinaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(BinaryOperationType, none);
                ADD_TO_STRING_ENTRY(BinaryOperationType, add);
                ADD_TO_STRING_ENTRY(BinaryOperationType, subtract);
                ADD_TO_STRING_ENTRY(BinaryOperationType, coordinatewiseMultiply);
                ADD_TO_STRING_ENTRY(BinaryOperationType, coordinatewiseDivide);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalAnd);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalOr);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalXor);
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
            }
        }

        inline BinaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(BinaryOperationType, none);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, add);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, subtract);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, coordinatewiseMultiply);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, coordinatewiseDivide);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalAnd);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalOr);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalXor);

            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
        }

        template <typename ValueType>
        ValueType Add(ValueType a, ValueType b)
        {
            return a + b;
        }

        template <>
        inline bool Add(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Subtract(ValueType a, ValueType b)
        {
            return a - b;
        }

        template <>
        inline bool Subtract(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Multiply(ValueType a, ValueType b)
        {
            return a * b;
        }

        template <>
        inline bool Multiply(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Divide(ValueType a, ValueType b)
        {
            return a / b;
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
            return a && b;
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
    BinaryOperationNode<ValueType>::BinaryOperationNode()
        : Node({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _operation(BinaryOperationType::none)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, BinaryOperationType operation)
        : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, _input1.Size()), _operation(operation)
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
            case BinaryOperationType::add:
                output = ComputeOutput(BinaryOperations::Add<ValueType>);
                break;
            case BinaryOperationType::subtract:
                output = ComputeOutput(BinaryOperations::Subtract<ValueType>);
                break;
            case BinaryOperationType::coordinatewiseMultiply:
                output = ComputeOutput(BinaryOperations::Multiply<ValueType>);
                break;
            case BinaryOperationType::coordinatewiseDivide:
                output = ComputeOutput(BinaryOperations::Divide<ValueType>);
                break;
            case BinaryOperationType::logicalAnd:
                output = ComputeOutput(BinaryOperations::LogicalAnd<ValueType>);
                break;
            case BinaryOperationType::logicalOr:
                output = ComputeOutput(BinaryOperations::LogicalOr<ValueType>);
                break;
            case BinaryOperationType::logicalXor:
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
    void BinaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[input1PortName] << _input1;
        archiver[input2PortName] << _input2;
        archiver[outputPortName] << _output;
        archiver["operation"] << BinaryOperations::to_string(_operation);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[input1PortName] >> _input1;
        archiver[input2PortName] >> _input2;
        archiver[outputPortName] >> _output;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = BinaryOperations::from_string(operation);
    }
}
}
