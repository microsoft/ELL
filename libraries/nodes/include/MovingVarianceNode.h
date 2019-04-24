////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MovingVarianceNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/InputPort.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that takes a vector input and returns its variance over some window of time. </summary>
    template <typename ValueType>
    class MovingVarianceNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MovingVarianceNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the variance of </param>
        /// <param name="windowSize"> The number of samples of history to use in computing the variance </param>
        MovingVarianceNode(const model::OutputPort<ValueType>& input, size_t windowSize);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MovingVarianceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

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
        mutable std::vector<ValueType> _runningSquaredSum;
        size_t _windowSize;
    };

    template <typename ValueType>
    const model::OutputPort<ValueType>& MovingVariance(const model::OutputPort<ValueType>& input, size_t windowSize);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    MovingVarianceNode<ValueType>::MovingVarianceNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _windowSize(0)
    {
    }

    template <typename ValueType>
    MovingVarianceNode<ValueType>::MovingVarianceNode(const model::OutputPort<ValueType>& input, size_t windowSize) :
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
        _runningSquaredSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Compute() const
    {
        static auto squared = [](const ValueType& x) { return x * x; };

        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for (size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index] - lastBufferedSample[index]);
            _runningSquaredSum[index] += squared(inputSample[index]) - squared(lastBufferedSample[index]);
            result[index] = (_runningSquaredSum[index] - (squared(_runningSum[index]) / _windowSize)) / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<MovingVarianceNode<ValueType>>(newInputs, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void MovingVarianceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        std::generate_n(std::back_inserter(_samples), _windowSize, [dimension] { return std::vector<ValueType>(dimension); });
        _runningSum = std::vector<ValueType>(dimension);
        _runningSquaredSum = std::vector<ValueType>(dimension);
        _output.SetSize(dimension);
    }

    template <typename ValueType>
    const model::OutputPort<ValueType>& MovingVariance(const model::OutputPort<ValueType>& input, size_t windowSize)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<MovingVarianceNode<ValueType>>(input, windowSize);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
