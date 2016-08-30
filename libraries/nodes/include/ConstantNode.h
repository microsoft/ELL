////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// predictors
#include "ConstantPredictor.h"

// utilities
#include "Serializer.h"
#include "TypeName.h"
#include "ObjectDescription.h"

// stl
#include <memory>
#include <vector>


/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that contains a constant value. Has no inputs. </summary>
    template <typename ValueType>
    class ConstantNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        ConstantNode();

        /// <summary> Constructor for a scalar constant </summary>
        ///
        /// <param name="value"> The scalar value </param>
        ConstantNode(ValueType value);

        /// Constructor for a vector constant
        ///
        /// <param name="value"> The vector value </param>
        ConstantNode(const std::vector<ValueType>& values);

        /// <summary> Gets the values contained in this node </summary>
        ///
        /// <returns> The values contained in this node </returns>
        const std::vector<ValueType>& GetValues() { return _values; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ConstantNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

        static utilities::ObjectDescription GetTypeDescription();
        virtual utilities::ObjectDescription GetDescription() const;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Output
        model::OutputPort<ValueType> _output;

        // Constant value
        std::vector<ValueType> _values;
    };

    /// <summary> Adds a constant node (which represents a constant predictor) to a model transformer. </summary>
    ///
    /// <param name="input"> The input to the predictor, which is ignored. </param>
    /// <param name="predictor"> The constant predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    ConstantNode<double>* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ConstantPredictor& predictor, model::ModelTransformer& transformer);
}

#include "../tcc/ConstantNode.tcc"
