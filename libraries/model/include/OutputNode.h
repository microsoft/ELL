////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelTransformer.h"
#include "OutputNodeBase.h"
#include "OutputPort.h"

#include <utilities/include/IArchivable.h>

#include <memory>
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{

    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public OutputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* shapeName = "shape";
        const InputPort<ValueType>& input = _input;
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The port to get the input data from </param>
        OutputNode(const OutputPort<ValueType>& input);

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The port to get the input data from </param>
        /// <param name="shape"> The shape of the output data </param>
        OutputNode(const OutputPort<ValueType>& input, const MemoryShape& shape);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        void Compute() const override;
        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;

    private:
        void Copy(ModelTransformer& transformer) const override;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="input"> The port to get the input data from </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ModelLikeType, typename ValueType>
    const OutputPort<ValueType>& AppendOutput(ModelLikeType& model, const OutputPort<ValueType>& input);
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename ValueType>
    OutputNode<ValueType>::OutputNode() :
        OutputNodeBase(_input, _output, {}),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
        SetShape({});
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const OutputPort<ValueType>& input) :
        OutputNodeBase(_input, _output, MemoryShape{ static_cast<int>(input.Size()) }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(MemoryShape{ static_cast<int>(input.Size()) });
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const OutputPort<ValueType>& input, const MemoryShape& shape) :
        OutputNodeBase(_input, _output, shape),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(shape);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newInputs, GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["layout"] << _input.GetMemoryLayout();
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;

        int size;
        archiver.OptionalProperty("size", 0) >> size;
        std::vector<int> shapeVector;
        archiver.OptionalProperty("shape", std::vector<int>{ size }) >> shapeVector;
        if (archiver.HasNextPropertyName("layout"))
        {
            PortMemoryLayout layout;
            archiver["layout"] >> layout;
            SetShape(layout.GetActiveSize());
        }
        else
        {
            SetShape({ shapeVector });
        }
    }

    template <typename ModelLikeType, typename ValueType>
    const OutputPort<ValueType>& AppendOutput(ModelLikeType& model, const OutputPort<ValueType>& input)
    {
        static_assert(std::is_same_v<ModelLikeType, model::Model> || std::is_same_v<ModelLikeType, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        auto node = model.template AddNode<OutputNode<ValueType>>(input);
        return node->output;
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
