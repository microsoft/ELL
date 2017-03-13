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
    void SumNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (IsPureVector(input) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pSrcVector = compiler.EnsurePortEmitted(input);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        function.Store(pResult, function.Literal(0.0));
        auto forLoop = function.ForLoop();
        forLoop.Begin(input.Size());
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pValue = function.ValueAt(pSrcVector, i);
            function.OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pValue);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        function.Store(pResult, function.Literal(0.0));
        for (size_t i = 0; i < input.Size(); ++i)
        {
            llvm::Value* pValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pValue);
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
    }

    template <typename ValueType>
    void SumNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
    }
}
}
