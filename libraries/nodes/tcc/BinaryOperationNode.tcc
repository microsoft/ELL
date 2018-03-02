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
        inline std::string to_string(emitters::BinaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, none);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, add);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, subtract);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseMultiply);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseDivide);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalAnd);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalOr);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalXor);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
            }
        }

        inline emitters::BinaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, none);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, add);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, subtract);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseMultiply);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseDivide);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalAnd);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalOr);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalXor);

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
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 0), _operation(emitters::BinaryOperationType::none)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, emitters::BinaryOperationType operation)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _output(this, defaultOutputPortName, _input1.Size()), _operation(operation)
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
        case emitters::BinaryOperationType::add:
            output = ComputeOutput(BinaryOperations::Add<ValueType>);
            break;
        case emitters::BinaryOperationType::subtract:
            output = ComputeOutput(BinaryOperations::Subtract<ValueType>);
            break;
        case emitters::BinaryOperationType::coordinatewiseMultiply:
            output = ComputeOutput(BinaryOperations::Multiply<ValueType>);
            break;
        case emitters::BinaryOperationType::coordinatewiseDivide:
            output = ComputeOutput(BinaryOperations::Divide<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalAnd:
            output = ComputeOutput(BinaryOperations::LogicalAnd<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalOr:
            output = ComputeOutput(BinaryOperations::LogicalOr<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalXor:
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
    void BinaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (IsPureVector(input1) && IsPureVector(input2) && !compiler.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        function.VectorOperator(emitters::GetOperator<ValueType>(GetOperation()), count, pInput1, pInput2, [&pResult, &function](llvm::Value* i, llvm::Value* pValue) {
            function.SetValueAt(pResult, i, pValue);
        });
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        for (size_t i = 0; i < count; ++i)
        {
            llvm::Value* inputValue1 = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            llvm::Value* inputValue2 = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            llvm::Value* pOpResult = function.Operator(emitters::GetOperator<ValueType>(GetOperation()), inputValue1, inputValue2);
            function.SetValueAt(pResult, function.Literal<int>(i), pOpResult);
        }
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver["operation"] << BinaryOperations::to_string(_operation);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = BinaryOperations::from_string(operation);
        _output.SetSize(_input1.Size());
    }
}
}
