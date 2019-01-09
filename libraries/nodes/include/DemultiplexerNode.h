////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DemultiplexerNode.h (node)
//  Authors:  ChuckJacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "MultiplexerNode.h"
#include "TypeCastNode.h"

#include <model/include/InputPort.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <utilities/include/TypeName.h>

#include <exception>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace nodes
{
    /// <summary> A node that routes its scalar input to one element of its outputs, depending on a separate selector input. The element at the index
    /// provided by `selector` is set to the input value, and the rest are set to a default value. </summary>
    template <typename ValueType, typename SelectorType>
    class DemultiplexerNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* selectorPortName = "selector";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<SelectorType>& selector = _selector;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DemultiplexerNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="selector"> The index of the chosen element to recieve the value </param>
        DemultiplexerNode(const model::OutputPort<ValueType>& input, const model::OutputPort<SelectorType>& selector, size_t outputSize, ValueType defaultValue = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, SelectorType>("DemultiplexerNode"); }

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
        bool HasState() const override { return true; } // stored state: default value

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;
        model::InputPort<SelectorType> _selector;

        // Output
        model::OutputPort<ValueType> _output;

        // Default value
        ValueType _defaultValue;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode() :
        Node({ &_input, &_selector }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _selector(this, {}, selectorPortName),
        _output(this, defaultOutputPortName, 0),
        _defaultValue(0)
    {
    }

    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode(const model::OutputPort<ValueType>& input, const model::OutputPort<SelectorType>& selector, size_t outputSize, ValueType defaultValue) :
        Node({ &_input, &_selector }, { &_output }),
        _input(this, input, defaultInputPortName),
        _selector(this, selector, selectorPortName),
        _output(this, defaultOutputPortName, outputSize),
        _defaultValue(defaultValue)
    {
        if (selector.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Condition must be 1-D signal");
        }
        if (input.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Input must be 1-D signal");
        }
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Compute() const
    {
        std::vector<ValueType> outputValue(_output.Size(), _defaultValue);
        int index = (int)_selector[0];
        outputValue[index] = _input[0];
        _output.SetOutput(outputValue);
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[selectorPortName] << _selector;
        archiver["size"] << _output.Size();
        archiver["defaultValue"] << _defaultValue;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[selectorPortName] >> _selector;
        size_t size;
        archiver["size"] >> size;
        _output.SetSize(size);
        archiver["defaultValue"] >> _defaultValue;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        const auto& newSelector = transformer.GetCorrespondingInputs(_selector);
        auto newNode = transformer.AddNode<DemultiplexerNode<ValueType, SelectorType>>(newInput, newSelector, output.Size(), _defaultValue);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    const model::OutputPort<int>& CastIfNecessary(const model::OutputPort<ValueType>& values, model::ModelTransformer& transformer)
    {
        auto castNode = transformer.AddNode<TypeCastNode<ValueType, int>>(values);
        return castNode->output;
    }

    template <>
    inline const model::OutputPort<int>& CastIfNecessary<int>(const model::OutputPort<int>& values, model::ModelTransformer& transformer)
    {
        return values;
    }

    template <typename ValueType, typename SelectorType>
    bool DemultiplexerNode<ValueType, SelectorType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        const auto& newSelector = transformer.GetCorrespondingInputs(_selector);
        const auto& newSelectorInt = CastIfNecessary(newSelector, transformer);

        const auto& defaultOutput = AppendConstant(transformer, _defaultValue);
        model::PortElements<ValueType> outputElements;
        auto size = _output.Size();
        for (size_t index = 0; index < size; ++index)
        {
            const auto& indexValue = AppendConstant(transformer, static_cast<int>(index));
            auto isEqualNode = transformer.AddNode<BinaryPredicateNode<int>>(newSelectorInt, indexValue, emitters::BinaryPredicateType::equal);
            auto ifNode = transformer.AddNode<nodes::MultiplexerNode<ValueType, bool>>(model::PortElements<ValueType>{ defaultOutput, newInput }, isEqualNode->output);
            outputElements.Append(ifNode->output);
        }

        const auto& newOutput = transformer.SimplifyOutputs(outputElements);
        transformer.MapNodeOutput(output, newOutput);
        return true;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
