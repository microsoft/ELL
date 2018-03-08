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
    DotProductNode<ValueType>::DotProductNode()
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _output(this, defaultOutputPortName, 1)
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
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<DotProductNode<ValueType>>(newInput1, newInput2);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DotProductNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // Maybe... in reality, dot product will likely want to be computed as in Compute() above
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto multNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newInput1, newInput2, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto sumNode = transformer.AddNode<SumNode<ValueType>>(multNode->output);

        transformer.MapNodeOutput(output, sumNode->output);
        return true;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean dot product nodes");
        if ((IsPureVector(input1) && IsPureVector(input2)) && !compiler.GetCompilerOptions().unrollLoops)
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
        llvm::Value* pLVector = compiler.EnsurePortEmitted(input1);
        llvm::Value* pRVector = compiler.EnsurePortEmitted(input2);
        int count = static_cast<int>(input1.Size());
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);
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
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        function.StoreZero(pResult);
        for (size_t i = 0; i < input1.Size(); ++i)
        {
            llvm::Value* pLeftValue = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            llvm::Value* pRightValue = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            llvm::Value* pMultiplyResult = function.Operator(emitters::GetMultiplyForValueType<ValueType>(), pLeftValue, pRightValue);
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
}
}
