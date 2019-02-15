////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeCastNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <string>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that represents an output from the system. </summary>
    template <typename InputValueType, typename OutputValueType>
    class TypeCastNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<InputValueType>& input = _input;
        const model::OutputPort<OutputValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        TypeCastNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        TypeCastNode(const model::OutputPort<InputValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputValueType, OutputValueType>("TypeCastNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        model::InputPort<InputValueType> _input;
        model::OutputPort<OutputValueType> _output;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0){};

    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode(const model::OutputPort<InputValueType>& input) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size()){};

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

        if (!function.GetCompilerOptions().unrollLoops)
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
            emitters::LLVMValue castElement = function.CastValue<OutputValueType>(inputValue);
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
            emitters::LLVMValue castElement = function.CastValue<OutputValueType>(inputValue);
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
} // namespace nodes
} // namespace ell

#pragma endregion implementation
