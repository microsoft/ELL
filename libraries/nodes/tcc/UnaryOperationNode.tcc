////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs, Kern Handa
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
    namespace UnaryOperations
    {
        inline std::string to_string(emitters::UnaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, none);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, sqrt);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, logicalNot);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, tanh);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, exp);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, square);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, log);

                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
            }
        }

        inline emitters::UnaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, none);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, sqrt);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, logicalNot);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, tanh);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, exp);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, square);
            ADD_FROM_STRING_ENTRY(emitters::UnaryOperationType, log);

            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
        }

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

        template <typename ValueType>
        ValueType Tanh(ValueType a)
        {
            return std::tanh(a);
        }

        template <>
        inline bool Tanh(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking tanh of a boolean value");
        }

        template <typename ValueType>
        ValueType Exp(ValueType a)
        {
            return std::exp(a);
        }

        template <>
        inline bool Exp(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking exp of a boolean value");
        }

        template <typename ValueType>
        ValueType Square(ValueType a)
        {
            return a * a;
        }

        template <>
        inline bool Square(bool)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking square of a boolean value");
        }

        template <typename ValueType>
        ValueType Log(ValueType a)
        {
            return std::log(a);
        }

        template <>
        inline bool Log(bool)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking log of a boolean value");
        }
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _operation(emitters::UnaryOperationType::none)
    {
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::PortElements<ValueType>& input, emitters::UnaryOperationType operation)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size()), _operation(operation)
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
        case emitters::UnaryOperationType::sqrt:
            output = ComputeOutput(UnaryOperations::Sqrt<ValueType>);
            break;
        case emitters::UnaryOperationType::logicalNot:
            output = ComputeOutput(UnaryOperations::LogicalNot<ValueType>);
            break;
        case emitters::UnaryOperationType::exp:
            output = ComputeOutput(UnaryOperations::Exp<ValueType>);
            break;
        case emitters::UnaryOperationType::tanh:
            output = ComputeOutput(UnaryOperations::Tanh<ValueType>);
            break;
        case emitters::UnaryOperationType::square:
            output = ComputeOutput(UnaryOperations::Square<ValueType>);
            break;
        case emitters::UnaryOperationType::log:
            output = ComputeOutput(UnaryOperations::Log<ValueType>);
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
    llvm::Function* UnaryOperationNode<ValueType>::GetOperator(emitters::IRFunctionEmitter& function) const
    {
        switch (this->GetOperation())
        {
            case emitters::UnaryOperationType::sqrt:
                return function.GetModule().GetRuntime().GetSqrtFunction<ValueType>();
            case emitters::UnaryOperationType::exp:
                return function.GetModule().GetRuntime().GetExpFunction<ValueType>();
            case emitters::UnaryOperationType::log:
                return function.GetModule().GetRuntime().GetLogFunction<ValueType>();
            case emitters::UnaryOperationType::logicalNot:
            {
                auto& module = function.GetModule();
                auto& f = module.BeginFunction("logicalNot", emitters::GetVariableType<bool>(), {emitters::GetVariableType<ValueType>()});
                auto args = f.Arguments().begin();
                llvm::Argument& val = *args;
                f.Return(f.LogicalNot(&val));
                module.EndFunction();
                return f.GetFunction();
            }
            case emitters::UnaryOperationType::square:
            {
                auto& module = function.GetModule();
                auto& f = module.BeginFunction("square", emitters::GetVariableType<ValueType>(), { emitters::GetVariableType<ValueType>() });
                auto args = f.Arguments().begin();
                llvm::Argument& val = *args;
                f.Return(f.Operator(emitters::GetMultiplyForValueType<ValueType>(), &val, &val));
                module.EndFunction();
                return f.GetFunction();
            }
            case emitters::UnaryOperationType::tanh:
                return function.GetModule().GetRuntime().GetTanhFunction<ValueType>();
            case emitters::UnaryOperationType::none:
            default:
                throw emitters::EmitterException(emitters::EmitterError::unaryOperationNotSupported);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (IsPureVector(input) && !compiler.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Loop version broken
        auto count = input.Size();
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto forLoop = function.ForLoop();
        forLoop.Begin(count);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* inputValue = function.ValueAt(pInput, i);
            llvm::Value* pOpResult = function.Call(GetOperator(function), { inputValue });
            function.SetValueAt(pResult, i, pOpResult);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        for (size_t i = 0; i < input.Size(); ++i)
        {
            llvm::Value* inputValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            llvm::Value* pOpResult = function.Call(GetOperator(function), { inputValue });
            function.SetValueAt(pResult, function.Literal((int)i), pOpResult);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["operation"] << UnaryOperations::to_string(_operation);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = UnaryOperations::from_string(operation);
        _output.SetSize(_input.Size());
    }
}
}
