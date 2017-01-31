////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SumNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    SumNode<ValueType>::SumNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    SumNode<ValueType>::SumNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += v;
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void SumNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SumNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compile(model::IRMapCompiler& compiler)
    {
        compiler.NewBlockRegion(*this);

        // SumNode has exactly 1 input and 1 output
        auto pInput = this->GetInputPorts()[0];
        if (IsPureVector(*pInput) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileSumLoop(compiler);
        }
        else
        {
            CompileSumExpanded(compiler);
        }

        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileSumLoop(model::IRMapCompiler& compiler)
    {
        auto pInput = this->GetInputPorts()[0];
        auto pOutput = this->GetOutputPorts()[0];
        llvm::Value* pSrcVector = compiler.EnsureEmitted(pInput);
        llvm::Value* pResult = compiler.EnsureEmitted(pOutput);
        // emitters::Variable& resultVar = *(compiler.GetVariableFor(pOutput));

        compiler.GetCurrentFunction().Store(pResult, compiler.GetCurrentFunction().Literal(0.0));
        auto forLoop = compiler.GetCurrentFunction().ForLoop();
        // auto pBodyBlock = forLoop.Begin(pInput->Size());
        forLoop.Begin(pInput->Size());
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pValue = compiler.GetCurrentFunction().ValueAt(pSrcVector, i);
            compiler.GetCurrentFunction().OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pValue);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileSumExpanded(model::IRMapCompiler& compiler)
    {
        auto pInput = this->GetInputPorts()[0];
        auto pOutput = this->GetOutputPorts()[0];
        llvm::Value* pResult = compiler.EnsureEmitted(pOutput);
        // emitters::Variable& resultVar = *(compiler.GetVariableFor(pOutput));

        compiler.GetCurrentFunction().Store(pResult, compiler.GetCurrentFunction().Literal(0.0));
        for (size_t i = 0; i < pInput->Size(); ++i)
        {
            llvm::Value* pValue = compiler.LoadVariable(pInput->GetInputElement(i));
            compiler.GetCurrentFunction().OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pValue);
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType>
    void SumNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }
}
}
