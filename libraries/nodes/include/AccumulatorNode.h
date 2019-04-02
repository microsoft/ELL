////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AccumulatorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <emitters/include/LLVMUtilities.h>

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that accumulates a running sum of its input. </summary>
    template <typename ValueType>
    class AccumulatorNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        AccumulatorNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to accumulate </param>
        AccumulatorNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("AccumulatorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue accumulator);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue accumulator);

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<ValueType> _accumulator;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="input"> The port to get the input data from </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const model::OutputPort<ValueType>& Accumulate(const model::OutputPort<ValueType>& input);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode(const model::OutputPort<ValueType>& input) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.Size())
    {
        auto dimension = input.Size();
        _accumulator = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Compute() const
    {
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            _accumulator[index] += _input[index];
        }
        _output.SetOutput(_accumulator);
    };

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<AccumulatorNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean accumulator nodes");
        assert(GetPortVariableType(input) == GetPortVariableType(output));

        // Allocate a global variable to accumulate the input
        emitters::Variable* pAccumulatorVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        emitters::LLVMValue accumulator = function.GetModule().EnsureEmitted(*pAccumulatorVar);

        if (!function.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function, accumulator);
        }
        else
        {
            CompileExpanded(compiler, function, accumulator);
        }
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue accumulator)
    {
        emitters::LLVMValue inputVector = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue result = compiler.EnsurePortEmitted(output);

        function.VectorOperator(emitters::GetAddForValueType<ValueType>(), output.Size(), accumulator, inputVector, [&accumulator, &result, &function](emitters::LLVMValue i, emitters::LLVMValue value) {
            function.SetValueAt(accumulator, i, value);
            function.SetValueAt(result, i, value);
        });
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue accumulator)
    {
        emitters::LLVMValue result = compiler.EnsurePortEmitted(output);
        for (size_t index = 0; index < output.Size(); ++index)
        {
            emitters::LLVMValue inputValue = compiler.LoadPortElementVariable(input.GetInputElement(index));
            emitters::LLVMValue accumValue = function.ValueAt(accumulator, function.Literal((int)index));
            emitters::LLVMValue sum = function.Operator(emitters::GetAddForValueType<ValueType>(), inputValue, accumValue);
            function.SetValueAt(accumulator, function.Literal((int)index), sum);
            function.SetValueAt(result, function.Literal((int)index), sum);
        }
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;

        auto dimension = _input.Size();
        _accumulator = std::vector<ValueType>(dimension);
        _output.SetSize(dimension);
    }

    template <typename ValueType>
    const model::OutputPort<ValueType>& Accumulate(const model::OutputPort<ValueType>& input)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<AccumulatorNode<ValueType>>(input);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
