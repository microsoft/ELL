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
        inline std::string to_string(emitters::BinaryPredicateType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, none);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, equal);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, less);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, greater);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, notEqual);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, lessOrEqual);
                ADD_TO_STRING_ENTRY(emitters::BinaryPredicateType, greaterOrEqual);
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary predicate");
            }
        }

        inline emitters::BinaryPredicateType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, none);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, equal);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, less);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, greater);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, notEqual);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, lessOrEqual);
            ADD_FROM_STRING_ENTRY(emitters::BinaryPredicateType, greaterOrEqual);

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
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 0), _predicate(emitters::BinaryPredicateType::none)
    {
    }

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, emitters::BinaryPredicateType predicate)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _output(this, defaultOutputPortName, _input1.Size()), _predicate(predicate)
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
            case emitters::BinaryPredicateType::equal:
                output = ComputeOutput(BinaryPredicates::Equal<ValueType>);
                break;
            case emitters::BinaryPredicateType::less:
                output = ComputeOutput(BinaryPredicates::Less<ValueType>);
                break;
            case emitters::BinaryPredicateType::greater:
                output = ComputeOutput(BinaryPredicates::Greater<ValueType>);
                break;
            case emitters::BinaryPredicateType::notEqual:
                output = ComputeOutput(BinaryPredicates::NotEqual<ValueType>);
                break;
            case emitters::BinaryPredicateType::lessOrEqual:
                output = ComputeOutput(BinaryPredicates::LessOrEqual<ValueType>);
                break;
            case emitters::BinaryPredicateType::greaterOrEqual:
                output = ComputeOutput(BinaryPredicates::GreaterOrEqual<ValueType>);
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
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
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
    void BinaryPredicateNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);
        emitters::TypedComparison cmp = emitters::GetComparison<ValueType>(GetPredicate());

        auto forLoop = function.ForLoop();
        forLoop.Begin(input1.Size());
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* inputValue1 = function.ValueAt(pInput1, i);
            llvm::Value* inputValue2 = function.ValueAt(pInput2, i);
            llvm::Value* pOpResult = function.Comparison(cmp, inputValue1, inputValue2);
            // LLVM internally uses 1 bit for boolean. We use integers to store boolean results (see CompileElementSelector). That requires a typecast in LLVM
            function.SetValueAt(pResult, i, function.CastBoolToByte(pOpResult));
        }
        forLoop.End();
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        for (size_t i = 0; i < count; ++i)
        {
            llvm::Value* inputValue1 = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            llvm::Value* inputValue2 = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            llvm::Value* pOpResult = function.Comparison(emitters::GetComparison<ValueType>(GetPredicate()), inputValue1, inputValue2);
            
            function.SetValueAt(pResult, function.Literal((int)i), function.CastBoolToByte(pOpResult));
        }
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver["predicate"] << BinaryPredicates::to_string(_predicate);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        std::string predicate;
        archiver["predicate"] >> predicate;
        _predicate = BinaryPredicates::from_string(predicate);
        _output.SetSize(_input1.Size());
    }
}
}
