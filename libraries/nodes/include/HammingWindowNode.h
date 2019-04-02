////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HammingWindowNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinaryOperationNode.h"
#include "ConstantNode.h"

#include <dsp/include/WindowFunctions.h>

#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that contains data for a Hamming window. </summary>
    ///
    /// <typeparam name="ValueType"> The element type. </typeparam>
    ///
    template <typename ValueType>
    class HammingWindowNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        HammingWindowNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to apply the window to. </param>
        HammingWindowNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("HammingWindowNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    HammingWindowNode<ValueType>::HammingWindowNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    HammingWindowNode<ValueType>::HammingWindowNode(const model::OutputPort<ValueType>& input) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::Compute() const
    {
        auto size = _input.Size();
        auto window = dsp::HammingWindow<ValueType>(size);
        auto result = std::vector<ValueType>(size);
        for (size_t index = 0; index < size; index++)
        {
            result[index] = _input[index] * window[index];
        }
        _output.SetOutput(result);
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<HammingWindowNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool HammingWindowNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        const auto& windowValue = Constant(transformer, dsp::HammingWindow<ValueType>(_input.Size()), newPortElements.GetMemoryLayout());
        const auto& product = Multiply(newPortElements, windowValue);
        transformer.MapNodeOutput(output, product);
        return true;
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
