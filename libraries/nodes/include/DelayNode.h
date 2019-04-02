////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DelayNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that returns a delayed sample of the input. </summary>
    template <typename ValueType>
    class DelayNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DelayNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to delay. </param>
        /// <param name="windowSize"> The number of samples of delay to apply to the input data. </param>
        DelayNode(const model::OutputPort<ValueType>& input, size_t windowSize);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DelayNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary>Return the window size</summary>
        size_t GetWindowSize() const { return _windowSize; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<std::vector<ValueType>> _samples;
        size_t _windowSize;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="input"> The port to get the input data from </param>
    /// <param name="windowSize"> The number of samples of delay to apply to the input data. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ElementType>
    const model::OutputPort<ElementType>& Delay(const model::OutputPort<ElementType>& input,
                                                size_t windowSize);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DelayNode<ValueType>::DelayNode(const model::OutputPort<ValueType>& input, size_t windowSize) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.Size()),
        _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for (size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }

    template <typename ValueType>
    DelayNode<ValueType>::DelayNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _windowSize(0)
    {
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compute() const
    {
        auto lastBufferedSample = _samples[0];
        _samples.push_back(_input.GetValue());
        _samples.erase(_samples.begin());
        _output.SetOutput(lastBufferedSample);
    };

    template <typename ValueType>
    void DelayNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue result = compiler.EnsurePortEmitted(output);

        size_t sampleSize = output.Size();
        size_t windowSize = this->GetWindowSize();
        size_t bufferSize = sampleSize * windowSize;

        //
        // Delay nodes are always long lived - either globals or heap. Currently, we use globals
        // Each sample chunk is of size == sampleSize. The number of chunks we hold onto == windowSize
        // We need two buffers - one for the entire lot, one for the "last" chunk forwarded to the next operator
        //
        emitters::Variable* delayLineVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, bufferSize);
        emitters::LLVMValue delayLine = function.GetModule().EnsureEmitted(*delayLineVar);

        //
        // We implement a delay as a Shift Register
        //
        emitters::LLVMValue inputBuffer = compiler.EnsurePortEmitted(input);
        function.ShiftAndUpdate<ValueType>(delayLine, bufferSize, sampleSize, inputBuffer, result);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void DelayNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _output.SetSize(dimension);
    }

    template <typename ElementType>
    const model::OutputPort<ElementType>& Delay(const model::OutputPort<ElementType>& input,
                                                size_t windowSize)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }

        auto node = model->AddNode<DelayNode<ElementType>>(input, windowSize);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
