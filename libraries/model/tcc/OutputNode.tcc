////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ValueType>
    OutputNode<ValueType>::OutputNode()
        : OutputNodeBase(_input, _output), _input(this, {}, inputPortName), _output(this, outputPortName, 0){};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input)
        : OutputNodeBase(_input, _output), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()){};

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(GetPortVariableType(input) == GetPortVariableType(output));

        auto inputIsInputNode = (dynamic_cast<const InputNodeBase*>(input.GetInputElement(0).ReferencedPort()->GetNode()) != nullptr);
        // TODO: re-enable this branch when scalar port bug is fixed 
        if (IsPureVector(input) && input.Size() != 1 && output.Size() != 1 && !inputIsInputNode && false)
        {
            auto pVar = compiler.GetVariableForElement(input.GetInputElement(0));
            compiler.SetVariableForPort(output, pVar);
        }
        else
        {
            llvm::Value* pOutput = compiler.EnsurePortEmitted(output);
            if (input.Size() == 1)
            {
                llvm::Value* pVal = compiler.LoadPortElementVariable(input.GetInputElement(0));
                function.Store(pOutput, pVal);
            }
            else
            {
                auto inputElements = input.GetInputElements();
                int rangeStart = 0;
                for (auto range : inputElements.GetRanges())
                {
                    llvm::Value* pInput = compiler.EnsurePortEmitted(*range.ReferencedPort());
                    auto forLoop = function.ForLoop();
                    auto rangeSize = range.Size();
                    forLoop.Begin(rangeSize);
                    {
                        auto i = forLoop.LoadIterationVariable();
                        auto inputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal<int>(range.GetStartIndex()));
                        auto outputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal(rangeStart));
                        llvm::Value* pValue = function.ValueAt(pInput, inputIndex);
                        function.SetValueAt(pOutput, outputIndex, pValue);
                    }
                    forLoop.End();
                    rangeStart += rangeSize;
                }
            }
        }
    }

    template <typename ValueType>
    void OutputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
}
}
