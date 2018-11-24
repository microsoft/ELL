////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DotProductNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode() :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, {}, defaultInput1PortName),
        _input2(this, {}, defaultInput2PortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _input2(this, input2, defaultInput2PortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input1.Size(); ++index)
        {
            result += _input1[index] * _input2[index];
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void DotProductNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        auto newNode = transformer.AddNode<DotProductNode<ValueType>>(newInput1, newInput2);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DotProductNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // Maybe... in reality, dot product will likely want to be computed as in Compute() above
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        auto multNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newInput1, newInput2, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto sumNode = transformer.AddNode<SumNode<ValueType>>(multNode->output);

        transformer.MapNodeOutput(output, sumNode->output);
        return true;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean dot product nodes");
        if (!compiler.GetCompilerOptions().unrollLoops)
        {
            CompileDotProductLoop(compiler, function);
        }
        else
        {
            CompileDotProductExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pLVector = compiler.EnsurePortEmitted(input1);
        emitters::LLVMValue pRVector = compiler.EnsurePortEmitted(input2);
        int count = static_cast<int>(input1.Size());
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);
        if (compiler.GetCompilerOptions().inlineOperators)
        {
            function.DotProduct(count, pLVector, pRVector, pResult);
        }
        else
        {
            function.Call(function.GetModule().GetRuntime().GetDotProductFunction<ValueType>(), { function.Literal(count), function.PointerOffset(pLVector, 0), function.PointerOffset(pRVector, 0), function.PointerOffset(pResult, 0) });
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.StoreZero(pResult);
        for (size_t i = 0; i < input1.Size(); ++i)
        {
            emitters::LLVMValue pLeftValue = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            emitters::LLVMValue pRightValue = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            emitters::LLVMValue pMultiplyResult = function.Operator(emitters::GetMultiplyForValueType<ValueType>(), pLeftValue, pRightValue);
            function.OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pMultiplyResult);
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
    }
} // namespace nodes
} // namespace ell
