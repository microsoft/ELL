////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BufferNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputPort.h>

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that buffers combines or more input buffers returning a larger window over that input.
    /// On each new input the buffer is shifted left by the size of that input.  For example, if the input size
    /// is 8 and the windowSize is 16 and the inputs are given in the sequence i1, i2, i3, i4 then the output 
    /// of the buffer node will be [0 i1], [i1 i2], [i2, i3], [i3 i4].  So if you think of the input as a 
    /// series of values over time (like audio signal) then the BufferNode provides a sliding window over that
    /// input data.
    /// </summary>
    template <typename ValueType>
    class BufferNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        BufferNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to buffer. </param>
        /// <param name="windowSize"> The size of the output of this node which should not be smaller than the input size. </param>
        BufferNode(const model::OutputPort<ValueType>& input, size_t windowSize);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BufferNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Return the window size </summary>
        ///
        /// <returns> The window size </returns>
        size_t GetWindowSize() const { return _windowSize; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; } // stored state: windowSize
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<ValueType> _samples;
        size_t _windowSize;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    BufferNode<ValueType>::BufferNode(const model::OutputPort<ValueType>& input, size_t windowSize) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, windowSize),
        _windowSize(windowSize)
    {
        _samples.resize(windowSize);
    }

    template <typename ValueType>
    BufferNode<ValueType>::BufferNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _windowSize(0)
    {
    }

    template <typename ValueType>
    void BufferNode<ValueType>::Compute() const
    {
        auto inputSize = input.Size();
        if (inputSize > _samples.size())
        {
            inputSize = _samples.size();
        }
        auto offset = _samples.size() - inputSize;
        if (offset > 0)
        {
            // shift the buffer left by the input size to make room for new input
            std::copy_n(_samples.begin() + inputSize, offset, _samples.begin());
        }

        // Copy input to right hand side of the buffer
        const std::vector<ValueType>& input = _input.GetReferencedPort().GetOutput();
        std::copy_n(input.begin(), inputSize, _samples.begin() + offset);

        _output.SetOutput(_samples);
    };

    template <typename ValueType>
    void BufferNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<BufferNode<ValueType>>(newInputs, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BufferNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        int inputSize = input.Size();
        int windowSize = this->GetWindowSize();
        if (inputSize > windowSize)
        {
            inputSize = windowSize;
        }
        auto offset = windowSize - inputSize;
       
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        auto bufferVar = function.GetModule().Variables().AddVectorVariable<ValueType>(emitters::VariableScope::global, windowSize);
        function.GetModule().AllocateVariable(*bufferVar);
        emitters::LLVMValue buffer = function.GetModule().EnsureEmitted(*bufferVar);

        if (offset > 0)
        {
            // shift the buffer left by the input size to make room for new input
            function.MemoryMove<ValueType>(buffer, inputSize, 0, offset);
        }
        // Copy input to right hand side of the buffer
        function.MemoryCopy<ValueType>(pInput, 0, buffer, offset, inputSize);

        // Copy to output
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(output);
        function.MemoryCopy<ValueType>(buffer, 0, pOutput, 0, windowSize);
    }

    template <typename ValueType>
    void BufferNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void BufferNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        _samples.resize(_windowSize);
        _output.SetSize(_windowSize);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
