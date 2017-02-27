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
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, 1)
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
    void DotProductNode<ValueType>::Compile(model::IRMapCompiler& compiler)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean dot product nodes");
        compiler.NewBlockRegion(*this);

        auto pInput1 = this->GetInputPorts()[0];
        auto pInput2 = this->GetInputPorts()[1];
        if ((IsPureVector(*pInput1) && IsPureVector(*pInput2)) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileDotProductLoop(compiler);
        }
        else
        {
            CompileDotProductExpanded(compiler);
        }

        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductLoop(model::IRMapCompiler& compiler)
    {
        llvm::Value* pLVector = compiler.EnsureEmitted(this->GetInputPorts()[0]);
        llvm::Value* pRVector = compiler.EnsureEmitted(this->GetInputPorts()[1]);
        auto pOutput = this->GetOutputPorts()[0];
        int count = (int)(this->GetInputPorts()[0])->Size();
        llvm::Value* pResult = compiler.EnsureEmitted(pOutput);
        if (compiler.GetCompilerParameters().inlineOperators)
        {
            compiler.GetCurrentFunction().DotProductFloat(count, pLVector, pRVector, pResult);
        }
        else
        {
            compiler.GetCurrentFunction().Call(compiler.GetRuntime().GetDotProductFloatFunction(), { compiler.GetCurrentFunction().Literal(count), compiler.GetCurrentFunction().PointerOffset(pLVector, 0), compiler.GetCurrentFunction().PointerOffset(pRVector, 0), compiler.GetCurrentFunction().PointerOffset(pResult, 0) });
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductExpanded(model::IRMapCompiler& compiler)
    {
        auto pInput1 = this->GetInputPorts()[0];
        auto pInput2 = this->GetInputPorts()[1];
        auto pOutput = this->GetOutputPorts()[0];
        llvm::Value* pResult = compiler.EnsureEmitted(pOutput);
        // emitters::Variable& resultVar = *(GetVariableFor(pOutput));

        compiler.GetCurrentFunction().Store(pResult, compiler.GetCurrentFunction().Literal(0.0));
        for (size_t i = 0; i < pInput1->Size(); ++i)
        {
            llvm::Value* pLeftValue = compiler.LoadVariable(pInput1->GetInputElement(i));
            llvm::Value* pRightValue = compiler.LoadVariable(pInput2->GetInputElement(i));
            llvm::Value* pMultiplyResult = compiler.GetCurrentFunction().Operator(emitters::GetMultiplyForValueType<ValueType>(), pLeftValue, pRightValue);
            compiler.GetCurrentFunction().OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pMultiplyResult);
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[input1PortName] << _input1;
        archiver[input2PortName] << _input2;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[input1PortName] >> _input1;
        archiver[input2PortName] >> _input2;
    }
}
}
