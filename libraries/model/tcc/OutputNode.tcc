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
    void OutputNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Outputs are always long lived - either globals or heap. Currently, we use globals
        assert(GetPortVariableType(input) == GetPortVariableType(output));

        // If the input to this node is a "pure" input port (it only refers to one output port), then we can just
        // use the input instead.
        if (IsPureVector(input) && false) // TODO: Re-enable when scalar output port test working
        {
            auto pVar = compiler.GetVariableForElement(input.GetInputElement(0));
            compiler.SetVariableForPort(output, pVar);
        }
        else
        {
            llvm::Value* pResult = compiler.EnsurePortEmitted(output);
            if (input.Size() == 1)
            {
                llvm::Value* pVal = compiler.LoadPortElementVariable(input.GetInputElement(0));
                function.Store(pResult, pVal);
            }
            else
            {
                for (size_t i = 0; i < input.Size(); ++i)
                {
                    llvm::Value* pVal = compiler.LoadPortElementVariable(input.GetInputElement(i));
                    function.SetValueAt(pResult, function.Literal((int)i), pVal);
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
