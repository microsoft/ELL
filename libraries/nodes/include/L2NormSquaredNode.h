///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2NormSquaredNode.h (nodes)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"
#include "UnaryOperationNode.h"

#include <model/include/InputPort.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <utilities/include/TypeName.h>

#include <cmath>
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that takes a vector input and returns its magnitude squared </summary>
    template <typename ValueType>
    class L2NormSquaredNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        L2NormSquaredNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the squared magnitude of </param>
        L2NormSquaredNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("L2NormSquaredNode"); }

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
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
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
    L2NormSquaredNode<ValueType>::L2NormSquaredNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    L2NormSquaredNode<ValueType>::L2NormSquaredNode(const model::OutputPort<ValueType>& input) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v * v);
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<L2NormSquaredNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool L2NormSquaredNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        const auto& squaredInput = AppendUnaryOperation(newPortElements, UnaryOperationType::square);
        const auto& sum = AppendSum(squaredInput);

        transformer.MapNodeOutput(output, sum);
        return true;
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
