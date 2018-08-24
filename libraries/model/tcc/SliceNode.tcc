////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SliceNode.tcc (model)
//  Authors:  SliceNode
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Model.h"
#include "IRMapCompiler.h"
#include "ModelTransformer.h"

namespace ell
{
namespace model
{
    template <typename ValueType>
    SliceNode<ValueType>::SliceNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0) {};

    template <typename ValueType>
    SliceNode<ValueType>::SliceNode(const OutputPortBase* port, int start, int count)
        : CompilableNode({ &_input }, { &_output }), _input(this, PortElements<ValueType>{ *port }, defaultInputPortName), _output(this, defaultOutputPortName, port->GetMemoryLayout()), _largestDimensionStart(start), _largestDimensionCount(count)
    {
        auto layout = port->GetMemoryLayout();
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SliceNode<ValueType>>(newPortElements.GetRanges()[0].ReferencedPort(), _largestDimensionStart, _largestDimensionCount);
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
}
}
