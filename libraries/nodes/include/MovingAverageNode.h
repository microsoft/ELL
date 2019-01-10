////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MovingAverageNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"

#include <model/include/InputPort.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that takes a vector input and returns its mean over some window of time. </summary>
    template <typename ValueType>
    class MovingAverageNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MovingAverageNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the mean of </param>
        /// <param name="windowSize"> The number of samples of history to use in computing the mean </param>
        MovingAverageNode(const model::OutputPort<ValueType>& input, size_t windowSize);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MovingAverageNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<std::vector<ValueType>> _samples;
        mutable std::vector<ValueType> _runningSum;
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
    MovingAverageNode<ValueType>::MovingAverageNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _windowSize(0)
    {
    }

    template <typename ValueType>
    MovingAverageNode<ValueType>::MovingAverageNode(const model::OutputPort<ValueType>& input, size_t windowSize) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.Size()),
        _windowSize(windowSize)
    {
        auto dimension = _input.Size();
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Compute() const
    {
        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for (size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index] - lastBufferedSample[index]);
            result[index] = _runningSum[index] / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MovingAverageNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<MovingAverageNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool MovingAverageNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto delayNode = transformer.AddNode<DelayNode<ValueType>>(newPortElements, _windowSize);
        const auto& difference = AppendBinaryOperation(transformer, newPortElements, delayNode->output, nodes::BinaryOperationType::subtract);
        auto accumNode = transformer.AddNode<AccumulatorNode<ValueType>>(difference);
        std::vector<ValueType> literalN(newPortElements.Size(), (ValueType)_windowSize);
        const auto& denominator = AppendConstant(transformer, literalN);
        const auto& quotient = AppendBinaryOperation(transformer, accumNode->output, denominator, nodes::BinaryOperationType::divide);
        transformer.MapNodeOutput(output, quotient);
        return true;
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void MovingAverageNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
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
        _runningSum = std::vector<ValueType>(dimension);
        _output.SetSize(dimension);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
