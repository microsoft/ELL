////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReorderDataNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// model
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "OutputNode.h"

// stl
#include <vector>

namespace ell
{
namespace nodes
{
    //
    // ReorderDataNode
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const DataShape& inputShape, const DataShape& outputShape, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, outputShape.GetMemorySize()), _inputShape(inputShape), _outputShape(outputShape), _paddingValue(paddingValue)
    {
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ReorderDataNode>(newPortElements, _inputShape, _outputShape);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compute() const
    {
        int outputSize = _outputShape.GetMemorySize();
        std::vector<ValueType> output(outputSize, _paddingValue);

        // loop over output
        for (int z = 0; z < _outputShape.GetExtent(2); ++z)
        {
            for (int y = 0; y < _outputShape.GetExtent(1); ++y)
            {
                for (int x = 0; x < _outputShape.GetExtent(0); ++x)
                {
                    auto outputIndex = _outputShape.GetEntryOffset({ x, y, z });
                    if (_inputShape.IsOutOfBounds({ x, y, z }))
                    {
                        output[outputIndex] = static_cast<ValueType>(0);
                    }
                    else
                    {
                        auto inputIndex = _inputShape.GetEntryOffset({ x, y, z });
                        output[outputIndex] = _input[inputIndex];
                    }
                }
            }
        }

        _output.SetOutput(output);
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output, _paddingValue);

        int outputSize = _outputShape.GetMemorySize();
        auto zLoop = function.ForLoop();
        zLoop.Begin(_outputShape.GetExtent(2));
        {
            llvm::Value* z = zLoop.LoadIterationVariable();
            auto yLoop = function.ForLoop();
            yLoop.Begin(_outputShape.GetExtent(1));
            {
                llvm::Value* y = yLoop.LoadIterationVariable();
                for (int x = 0; x < _outputShape.GetExtent(0); ++x)
                {
                    auto outputIndex = _outputShape.EmitGetEntryOffset(function, { function.Literal<int>(x), y, z });
                    auto oob = _inputShape.EmitIsOutOfBounds(function, { function.Literal<int>(x), y, z });
                    auto ifOob = function.If();
                    ifOob.If(oob);
                    {
                        function.SetValueAt(pOutput, outputIndex, function.Literal<ValueType>(0));
                    }
                    ifOob.Else();
                    {
                        auto inputIndex = _inputShape.EmitGetEntryOffset(function, { function.Literal<int>(x), y, z });
                        llvm::Value* value = function.ValueAt(pInput, inputIndex);
                        function.SetValueAt(pOutput, outputIndex, value);
                    }
                    ifOob.End();
                }
            }
            yLoop.End();
        }
        zLoop.End();
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver["inputShape"] << _inputShape;
        archiver["outputShape"] << _outputShape;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver["inputShape"] >> _inputShape;
        archiver["outputShape"] >> _outputShape;
        archiver["paddingValue"] >> _paddingValue;
    }

} // nodes
} // ell
