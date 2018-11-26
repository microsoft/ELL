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
#include <model/include/PortElements.h>

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that buffers the input and allows access to the buffer. </summary>
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
        /// <param name="windowSize"> The number of samples to accumulate in the buffer. </param>
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
            // Copy samples forward to make room for new samples
            std::copy_n(_samples.begin() + offset, inputSize, _samples.begin());
        }
        // Copy input samples to tail
        for (size_t index = 0; index < inputSize; ++index)
        {
            _samples[index + offset] = _input[index];
        }
        _output.SetOutput(_samples);
    };

    template <typename ValueType>
    void BufferNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<BufferNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BufferNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        int inputSize = input.Size();
        size_t windowSize = this->GetWindowSize();
        auto offset = windowSize - inputSize;

        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        auto bufferVar = function.GetModule().Variables().AddVectorVariable<ValueType>(emitters::VariableScope::global, windowSize);
        function.GetModule().AllocateVariable(*bufferVar);
        emitters::LLVMValue buffer = function.GetModule().EnsureEmitted(*bufferVar);

        // Copy samples forward to make room for new samples
        function.MemoryMove<ValueType>(buffer, offset, 0, inputSize);

        // Copy input samples to tail
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
