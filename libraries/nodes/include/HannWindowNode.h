////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HannWindowNode.h (nodes)
//  Authors:  Chris Lovett
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
    /// <summary> A node that contains data for a Hann window. </summary>
    ///
    /// <typeparam name="ValueType"> The element type. </typeparam>
    /// <remarks> See https://en.wikipedia.org/wiki/Window_function#Hann_window </remarks>
    template <typename ValueType>
    class HannWindowNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        HannWindowNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to apply the window to. </param>
        HannWindowNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("HannWindowNode"); }

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
    HannWindowNode<ValueType>::HannWindowNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    HannWindowNode<ValueType>::HannWindowNode(const model::OutputPort<ValueType>& input) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
    }

    template <typename ValueType>
    void HannWindowNode<ValueType>::Compute() const
    {
        auto size = _input.Size();
        auto window = dsp::HannWindow<ValueType>(size);
        auto result = std::vector<ValueType>(size);
        for (size_t index = 0; index < size; index++)
        {
            result[index] = _input[index] * window[index];
        }
        _output.SetOutput(result);
    }

    template <typename ValueType>
    void HannWindowNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<HannWindowNode<ValueType>>(newInputs);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool HannWindowNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        const auto& windowValue = Constant(transformer, dsp::HannWindow<ValueType>(_input.Size()), newInputs.GetMemoryLayout());
        const auto& product = Multiply(newInputs, windowValue);
        transformer.MapNodeOutput(output, product);
        return true;
    }

    template <typename ValueType>
    void HannWindowNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void HannWindowNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
