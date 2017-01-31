////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
/// <summary> model namespace </summary>
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
    void OutputNode<ValueType>::Compile(IRMapCompiler& compiler)
    {
        // OutputNode has exactly 1 input and 1 output
        // Outputs are always long lived - either globals or heap. Currently, we use globals
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        assert(GetPortVariableType(*inputPort) == GetPortVariableType(*outputPort));

        auto& function = compiler.GetCurrentFunction();

        // Output ports have exactly 1 input, output
        llvm::Value* pOutputVar = compiler.EnsureEmitted(outputPort);
        for (size_t i = 0; i < inputPort->Size(); ++i)
        {
            llvm::Value* pVal = compiler.LoadVariable(inputPort->GetInputElement(i));
            function.SetValueAt(pOutputVar, function.Literal((int)i), pVal);
        }
    }

    template <typename ValueType>
    void OutputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }
}
}
