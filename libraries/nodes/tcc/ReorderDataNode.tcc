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
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout.GetMemorySize()), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _outputDimensionOrder({}), _paddingValue(paddingValue)
    {
        assert(_inputMemoryLayout.NumDimensions() == _outputMemoryLayout.NumDimensions());
        int numDimensions = _outputMemoryLayout.NumDimensions();
        _outputDimensionOrder.resize(numDimensions);
        for (int index = 0; index < numDimensions; ++index)
        {
            _outputDimensionOrder[index] = index;
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, const std::vector<int>& order, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout.GetMemorySize()), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _outputDimensionOrder(order), _paddingValue(paddingValue)
    {
        assert(_inputMemoryLayout.NumDimensions() == _outputMemoryLayout.NumDimensions());
        assert(_inputMemoryLayout.NumDimensions() == _outputDimensionOrder.size());
    }

    // Note: order {2, 0, 1} maps {row, column, channel} -> {channel, row, column}
    // So, we want to map entry 0  of the input (row) -> entry 1 (row) of the output
    template <typename ValueType>
    model::Shape ReorderDataNode<ValueType>::ReorderInputToOutputLocation(model::Shape inputLocation) const
    {
        model::Shape result;
        result.resize(inputLocation.size());
        for (int index = 0; index < result.size(); ++index)
        {
            result[index] = inputLocation[_outputDimensionOrder[index]];
        }
        return result;
    }

    template <typename ValueType>
    std::vector<llvm::Value*> ReorderDataNode<ValueType>::ReorderInputToOutputLocation(std::vector<llvm::Value*> inputLocation) const
    {
        std::vector<llvm::Value*> result;
        result.resize(inputLocation.size());
        for (int index = 0; index < result.size(); ++index)
        {
            result[index] = inputLocation[_outputDimensionOrder[index]];
        }
        return result;
    }

    // Note: order {2, 0, 1} maps {row, column, channel} -> {channel, row, column}
    // So, we want to map entry 0  of the output (channel) -> entry 2 (channel) of the intput
    template <typename ValueType>
    model::Shape ReorderDataNode<ValueType>::ReorderOutputToInputLocation(model::Shape outputLocation) const
    {
        model::Shape result;
        result.resize(outputLocation.size());
        for (size_t index = 0; index < result.size(); ++index)
        {
            result[_outputDimensionOrder[index]] = outputLocation[index];
        }
        return result;
    }

    template <typename ValueType>
    std::vector<llvm::Value*> ReorderDataNode<ValueType>::ReorderOutputToInputLocation(std::vector<llvm::Value*> outputLocation) const
    {
        std::vector<llvm::Value*> result;
        result.resize(outputLocation.size());
        for (size_t index = 0; index < result.size(); ++index)
        {
            result[_outputDimensionOrder[index]] = outputLocation[index];
        }
        return result;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ReorderDataNode>(newPortElements, _inputMemoryLayout, _outputMemoryLayout, _outputDimensionOrder, _paddingValue);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    // TODO: for each dimension, loop over minimum of input and output interval. Then we don't have to check if the value is out-of-bounds
    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compute() const
    {
        model::Shape inputIncrement = _inputMemoryLayout.GetCumulativeIncrement();
        model::Shape outputIncrement = _outputMemoryLayout.GetCumulativeIncrement();

        int outputSize = _outputMemoryLayout.GetMemorySize();
        std::vector<ValueType> output(outputSize, _paddingValue); // initialize to padding value

        // loop over output
        for (int x = 0; x < _outputMemoryLayout.GetActiveSize(0); ++x)
        {
            for (int y = 0; y < _outputMemoryLayout.GetActiveSize(1); ++y)
            {
                for (int z = 0; z < _outputMemoryLayout.GetActiveSize(2); ++z)
                {
                    auto inputLocation = ReorderOutputToInputLocation({ x, y, z });
                    auto inputIndex = _inputMemoryLayout.GetEntryOffset(inputLocation);
                    auto outputIndex = _outputMemoryLayout.GetEntryOffset({ x, y, z });
                    output[outputIndex] = _input[inputIndex];
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
        assert(this->input.Size() > 1);

        int outputSize = _outputMemoryLayout.GetMemorySize();
        UNUSED(outputSize);

        auto xLoop = function.ForLoop();
        xLoop.Begin(_outputMemoryLayout.GetActiveSize(0));
        {
            llvm::Value* x = xLoop.LoadIterationVariable();

            auto yLoop = function.ForLoop();
            yLoop.Begin(_outputMemoryLayout.GetActiveSize(1));
            {
                llvm::Value* y = yLoop.LoadIterationVariable();

                auto zLoop = function.ForLoop();
                zLoop.Begin(_outputMemoryLayout.GetActiveSize(2));
                {
                    llvm::Value* z = zLoop.LoadIterationVariable();
                    auto inputLocation = ReorderOutputToInputLocation({ x, y, z });
                    auto inputIndex = _inputMemoryLayout.EmitGetEntryOffset(function, inputLocation);
                    auto outputIndex = _outputMemoryLayout.EmitGetEntryOffset(function, { x, y, z });
                    llvm::Value* value = function.ValueAt(pInput, inputIndex);
                    function.SetValueAt(pOutput, outputIndex, value);
                }
                zLoop.End();
            }
            yLoop.End();
        }
        xLoop.End();
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << _outputMemoryLayout;
        archiver["order"] << _outputDimensionOrder;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        archiver["outputLayout"] >> _outputMemoryLayout;
        archiver["order"] >> _outputDimensionOrder;
        archiver["paddingValue"] >> _paddingValue;
    }

} // nodes
} // ell
