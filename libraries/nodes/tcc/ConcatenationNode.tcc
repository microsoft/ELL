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
    ConcatenationNode<ValueType>::ConcatenationNode(const model::OutputPort<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size()) {};

    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode(const model::OutputPort<ValueType>& input, const model::MemoryShape& shape)
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
        if (_input.Size() != 1 && _output.Size() != 1 && !inputIsInputNode && false)
        {
            auto pVar = compiler.GetVariableForPort(_input.GetReferencedPort());
            compiler.SetVariableForPort(_output, pVar);
        }
        else
        {
            auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
            auto output = function.LocalArray(compiler.EnsurePortEmitted(_output));
            // check if the output variable is null.
            function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [input, output, this](emitters::IRFunctionEmitter& function) {
                auto size = _input.Size();
                function.For(size, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                    output[i] = input[i];
                });
            });
        }
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
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
