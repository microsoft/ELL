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
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _outputDimensionOrder({}), _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        assert(_inputMemoryLayout.NumDimensions() == outputMemoryLayout.NumDimensions());
        int numDimensions = outputMemoryLayout.NumDimensions();
        _outputDimensionOrder.resize(numDimensions);
        for (int index = 0; index < numDimensions; ++index)
        {
            _outputDimensionOrder[index] = index;
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _outputDimensionOrder({}), _paddingValue(paddingValue)
    {
        assert(inputMemoryLayout.NumDimensions() == outputMemoryLayout.NumDimensions());
        int numDimensions = outputMemoryLayout.NumDimensions();
        _outputDimensionOrder.resize(numDimensions);
        for (int index = 0; index < numDimensions; ++index)
        {
            _outputDimensionOrder[index] = index;
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, const std::vector<int>& order, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _outputDimensionOrder(order), _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        assert(_inputMemoryLayout.NumDimensions() == outputMemoryLayout.NumDimensions());
        assert(_inputMemoryLayout.NumDimensions() == _outputDimensionOrder.size());
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, const std::vector<int>& order, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _outputDimensionOrder(order), _paddingValue(paddingValue)
    {
        assert(inputMemoryLayout.NumDimensions() == outputMemoryLayout.NumDimensions());
        assert(_inputMemoryLayout.NumDimensions() == _outputDimensionOrder.size());
    }

    // Note: order {2, 0, 1} maps {row, column, channel} -> {channel, row, column}
    // So, we want to map entry 0  of the input (row) -> entry 1 (row) of the output
    template <typename ValueType>
    model::MemoryShape ReorderDataNode<ValueType>::ReorderInputToOutputLocation(model::MemoryShape inputLocation) const
    {
        model::MemoryShape result;
        result.Resize(inputLocation.NumDimensions());
        for (int index = 0; index < result.NumDimensions(); ++index)
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
    model::MemoryShape ReorderDataNode<ValueType>::ReorderOutputToInputLocation(model::MemoryShape outputLocation) const
    {
        model::MemoryShape result;
        result.Resize(outputLocation.NumDimensions());
        for (int index = 0; index < result.NumDimensions(); ++index)
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
        auto newNode = transformer.AddNode<ReorderDataNode>(newPortElements, _inputMemoryLayout, _output.GetMemoryLayout(), _outputDimensionOrder, _paddingValue);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    // TODO: for each dimension, loop over minimum of input and output interval. Then we don't have to check if the value is out-of-bounds
    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compute() const
    {
        auto outputMemoryLayout = _output.GetMemoryLayout();
        model::MemoryShape inputIncrement = _inputMemoryLayout.GetCumulativeIncrement();
        model::MemoryShape outputIncrement = outputMemoryLayout.GetCumulativeIncrement();

        int outputSize = outputMemoryLayout.GetMemorySize();
        std::vector<ValueType> output(outputSize, _paddingValue); // initialize to padding value

        // loop over output
        for (int x = 0; x < outputMemoryLayout.GetActiveSize(0); ++x)
        {
            for (int y = 0; y < outputMemoryLayout.GetActiveSize(1); ++y)
            {
                for (int z = 0; z < outputMemoryLayout.GetActiveSize(2); ++z)
                {
                    auto inputLocation = ReorderOutputToInputLocation({ x, y, z });
                    auto inputIndex = _inputMemoryLayout.GetEntryOffset(inputLocation);
                    auto outputIndex = outputMemoryLayout.GetEntryOffset({ x, y, z });
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

        auto outputMemoryLayout = _output.GetMemoryLayout();
        int outputSize = outputMemoryLayout.GetMemorySize();
        UNUSED(outputSize);

        function.For(outputMemoryLayout.GetActiveSize(0), [outputMemoryLayout, pInput, pOutput, this](emitters::IRFunctionEmitter& function, llvm::Value* x) {
            function.For(outputMemoryLayout.GetActiveSize(1), [x, outputMemoryLayout, pInput, pOutput, this](emitters::IRFunctionEmitter& function, llvm::Value* y) {
                function.For(outputMemoryLayout.GetActiveSize(2), [x, y, outputMemoryLayout, pInput, pOutput, this](emitters::IRFunctionEmitter& function, llvm::Value* z) {
                    auto inputLocation = ReorderOutputToInputLocation({ x, y, z });
                    auto inputIndex = _inputMemoryLayout.EmitGetEntryOffset(function, inputLocation);
                    auto outputIndex = outputMemoryLayout.EmitGetEntryOffset(function, { x, y, z });
                    llvm::Value* value = function.ValueAt(pInput, inputIndex);
                    function.SetValueAt(pOutput, outputIndex, value);
                });
            });
        });
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["order"] << _outputDimensionOrder;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        model::PortMemoryLayout outputMemoryLayout;
        if (archiver.HasNextPropertyName("outputLayout"))
        {
            // backward-compatability
            archiver["outputLayout"] >> outputMemoryLayout;
            _output.SetMemoryLayout(outputMemoryLayout);
        }

        archiver["order"] >> _outputDimensionOrder;
        archiver["paddingValue"] >> _paddingValue;
    }

} // nodes
} // ell
