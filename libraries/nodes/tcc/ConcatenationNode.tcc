////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConcatenationNode.tcc (nodes)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0) {};

    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size()) {};

    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode(const model::PortElements<ValueType>& input, const model::MemoryShape& shape)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, shape) {};

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(GetPortVariableType(_input) == GetPortVariableType(_output));

        auto inputIsInputNode = (dynamic_cast<const model::InputNodeBase*>(_input.GetInputElement(0).ReferencedPort()->GetNode()) != nullptr);
        // TODO: re-enable this branch when scalar port bug is fixed
        if (IsPureVector(_input) && _input.Size() != 1 && _output.Size() != 1 && !inputIsInputNode && false)
        {
            auto pVar = compiler.GetVariableForElement(_input.GetInputElement(0));
            compiler.SetVariableForPort(_output, pVar);
        }
        else
        {
            emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(_output);
            // check if the pOutput variable is null.
            function.If(ell::emitters::TypedComparison::notEquals, pOutput, function.NullPointer(pOutput->getType()->getPointerElementType()->getPointerTo()), [pOutput, &compiler, this](emitters::IRFunctionEmitter& function) {
                auto inputElements = _input.GetInputElements();
                int rangeStart = 0;
                for (auto range : inputElements.GetRanges())
                {
                    emitters::LLVMValue pInput = compiler.EnsurePortEmitted(*range.ReferencedPort());
                    auto rangeSize = range.Size();

                    function.For(rangeSize, [=](emitters::IRFunctionEmitter& function, emitters::LLVMValue loopIndex) {
                        auto i = function.LocalScalar(loopIndex);
                        auto inputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal<int>(range.GetStartIndex()));
                        auto outputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal(rangeStart));
                        emitters::LLVMValue pValue = function.ValueAt(pInput, inputIndex);
                        function.SetValueAt(pOutput, outputIndex, pValue);
                    });
                    rangeStart += rangeSize;
                }
            });
        }
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ConcatenationNode<ValueType>>(newPortElements, GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[shapeName] << GetShape().ToVector();
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::vector<int> shapeVector;
        archiver[shapeName] >> shapeVector;
        _output.SetSize(_input.Size());
        if (shapeVector.size() >= 3)
        {
            SetShape({ shapeVector });
        }
    }
}
}
