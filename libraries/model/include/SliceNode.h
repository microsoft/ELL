////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SliceNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class IRMapCompiler;
    class ModelTransformer;

    /// <summary> A node that returns a subset of the entries from an output port. </summary>
    template <typename ValueType>
    class SliceNode : public CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const InputPort<ValueType>& input = _input;
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SliceNode();

        /// <summary> Constructor </summary>
        /// <param name="port"> The port to take input values from. </param>
        /// <param name="start"> The start index for the first (largest) physical dimension of the active area. </param>
        /// <param name="count"> The size of the first (largest) physical dimension of the output to return. </param>
        SliceNode(const OutputPortBase& port, int start, int count);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SliceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(ModelTransformer& transformer) const override;

        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
        int _largestDimensionStart = 0;
        int _largestDimensionCount = 0;
    };

    /// <summary> Convenience function for adding a SliceNode to a model. </summary>
    ///
    /// <param name="port"> The port to take input values from. </param>
    /// <param name="start"> The start index for the first (largest) physical dimension of the active area. </param>
    /// <param name="count"> The size of the first (largest) physical dimension of the output to return. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const OutputPort<ValueType>& Slice(const OutputPort<ValueType>& input, int start, int count);

} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename ValueType>
    SliceNode<ValueType>::SliceNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0){};

    template <typename ValueType>
    SliceNode<ValueType>::SliceNode(const OutputPortBase& port, int start, int count) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, static_cast<const OutputPort<ValueType>&>(port), defaultInputPortName),
        _output(this, defaultOutputPortName, port.GetMemoryLayout()),
        _largestDimensionStart(start),
        _largestDimensionCount(count)
    {
        auto layout = port.GetMemoryLayout();
        if (layout.HasPadding())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SliceNode must not have padding on its input");
        }

        auto newShape = layout.GetActiveSize();
        newShape[0] = _largestDimensionCount;
        _output.SetMemoryLayout({ newShape, layout.GetLogicalDimensionOrder() });
    }

    template <typename ValueType>
    void SliceNode<ValueType>::Compute() const
    {
        auto input = _input.GetValue();
        auto output = std::vector<ValueType>(input.begin() + _largestDimensionStart, input.begin() + _largestDimensionStart + _largestDimensionCount);
        _output.SetOutput(output);
    }

    template <typename ValueType>
    void SliceNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (GetPortVariableType(_input) != GetPortVariableType(_output))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Input and output port types must match");
        }

        auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
        auto output = function.LocalArray(compiler.EnsurePortEmitted(_output));

        auto layout = _input.GetReferencedPort().GetMemoryLayout();
        const auto increment = layout.GetCumulativeIncrement(0); // slowest-moving dimension
        const auto inputOffset = static_cast<int>(_largestDimensionStart * increment);
        const auto rangeSize = _largestDimensionCount * increment;
        function.For(rangeSize, [=](emitters::IRFunctionEmitter& function, emitters::IRLocalScalar i) {
            output[i] = input[inputOffset + i];
        });
    }

    template <typename ValueType>
    void SliceNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SliceNode<ValueType>>(newInputs, _largestDimensionStart, _largestDimensionCount);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SliceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["start"] << _largestDimensionStart;
        archiver["count"] << _largestDimensionCount;
        archiver["layout"] << _output.GetMemoryLayout();
    }

    template <typename ValueType>
    void SliceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["start"] >> _largestDimensionStart;
        archiver["count"] >> _largestDimensionCount;
        PortMemoryLayout layout;
        archiver["layout"] >> layout;
        _output.SetMemoryLayout(layout);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& Slice(const OutputPort<ValueType>& input, int start, int count)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<SliceNode<ValueType>>(input, start, count);
        return node->output;
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
