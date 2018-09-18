////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeCastNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0){};

    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode(const model::OutputPort<InputValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size()){};

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Compute() const
    {
        auto size = _output.Size();
        std::vector<OutputValueType> outputValues(size);
        for (size_t index = 0; index < size; ++index)
        {
            outputValues[index] = static_cast<OutputValueType>(_input[index]);
        }
        _output.SetOutput(outputValues);
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<TypeCastNode<InputValueType, OutputValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // The IR compiler currently implements bools using integers. We'll just use the already created variable.
        auto inputType = emitters::GetVariableType<InputValueType>();
        auto outputType = emitters::GetVariableType<OutputValueType>();

        // no-op case
        if (inputType == outputType)
        {
            emitters::Variable* elementVar = compiler.GetVariableForPort(input.GetReferencedPort());
            compiler.SetVariableForPort(output, elementVar); // The types are the same, so this is a no-op. Just set the output variable to be the same as the input variable
            return;
        }

        if (!compiler.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto count = input.Size();
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.For(count, [pInput, pResult](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            emitters::LLVMValue inputValue = function.ValueAt(pInput, i);
            emitters::LLVMValue castElement = function.CastValue<InputValueType, OutputValueType>(inputValue);
            function.SetValueAt(pResult, i, castElement);
        });
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        for (size_t i = 0; i < input.Size(); ++i)
        {
            emitters::LLVMValue inputValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            emitters::LLVMValue castElement = function.CastValue<InputValueType, OutputValueType>(inputValue);
            function.SetValueAt(pResult, function.Literal((int)i), castElement);
        }
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
}
}
