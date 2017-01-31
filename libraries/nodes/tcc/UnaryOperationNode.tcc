////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.tcc (nodes)
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
    namespace UnaryOperations
    {
        inline std::string to_string(emitters::UnaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, none);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, sqrt);
                ADD_TO_STRING_ENTRY(emitters::UnaryOperationType, logicalNot);

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
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _operation(emitters::UnaryOperationType::none)
    {
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::PortElements<ValueType>& input, emitters::UnaryOperationType operation)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _operation(operation)
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
            {
                output = ComputeOutput(UnaryOperations::Sqrt<ValueType>);
            }
            break;
            case emitters::UnaryOperationType::logicalNot:
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
    llvm::Function* UnaryOperationNode<ValueType>::GetOperator(model::IRMapCompiler& compiler) const
    {
        switch (this->GetOperation())
        {
            case emitters::UnaryOperationType::sqrt:
            {
                return compiler.GetRuntime().GetSqrtFunction<ValueType>();
            }
            break;
            default:
                throw emitters::EmitterException(emitters::EmitterError::unaryOperationNotSupported);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler)
    {
        compiler.NewBlockRegion(*this);

        auto inputPort = GetInputPorts()[0];

        if (IsPureVector(*inputPort) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileUnaryOperationLoop(compiler);
        }
        else
        {
            CompileUnaryOperationExpanded(compiler);
        }

        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileUnaryOperationLoop(model::IRMapCompiler& compiler)
    {
        // Loop version broken
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        auto count = inputPort->Size();
        llvm::Value* pInput = compiler.EnsureEmitted(inputPort);
        llvm::Value* pResult = compiler.EnsureEmitted(outputPort);
        auto& function = compiler.GetCurrentFunction();

        auto forLoop = function.ForLoop();
        forLoop.Begin(count);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* inputValue = function.ValueAt(pInput, i);
            llvm::Value* pOpResult = function.Call(GetOperator(compiler), { inputValue });
            function.SetValueAt(pResult, i, pOpResult);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileUnaryOperationExpanded(model::IRMapCompiler& compiler)
    {
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        llvm::Value* pResult = compiler.EnsureEmitted(outputPort);
        auto& function = compiler.GetCurrentFunction();

        for (size_t i = 0; i < inputPort->Size(); ++i)
        {
            llvm::Value* inputValue = compiler.LoadVariable(inputPort->GetInputElement(i));
            llvm::Value* pOpResult = function.Call(GetOperator(compiler), { inputValue });
            function.SetValueAt(pResult, function.Literal((int)i), pOpResult);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        archiver["operation"] << UnaryOperations::to_string(_operation);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = UnaryOperations::from_string(operation);
    }
}
}
