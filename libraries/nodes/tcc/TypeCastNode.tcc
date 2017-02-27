////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeCastNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
/// <summary> model namespace </summary>
namespace nodes
{
    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0){};

    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode(const model::PortElements<InputValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()){};

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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<TypeCastNode<InputValueType, OutputValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Compile(model::IRMapCompiler& compiler)
    {
        auto& function = compiler.GetCurrentFunction();
        // The IR compiler currently implements bools using integers. We'll just use the already created variable.

        auto inputType = emitters::GetVariableType<InputValueType>();
        auto outputType = emitters::GetVariableType<OutputValueType>();

        // Typecast has 1 input and 1 output port
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        VerifyIsScalar(*inputPort);
        VerifyIsScalar(*outputPort);

        if (inputType == outputType)
        {
            emitters::Variable* elementVar = compiler.GetVariableFor(inputPort->GetInputElement(0));
            compiler.SetVariableFor(outputPort, elementVar);
        }
        else
        {
            llvm::Value* inputValue = compiler.LoadVariable(inputPort->GetInputElement(0));
            llvm::Value* outputValue = compiler.EnsureEmitted(outputPort);

            llvm::Value* castElement = function.CastValue<InputValueType, OutputValueType>(inputValue);
            function.Store(outputValue, castElement);
        }
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
}
}
