////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictorNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "Model.h"
#include "ModelTransformer.h"

// predictors
#include "LinearPredictor.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> A node that represents a linear predictor. </summary>
    class LinearPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        static constexpr const char* weightedElementsPortName = "weightedElements";
        const model::OutputPort<double>& output = _output;
        const model::OutputPort<double>& weightedElements = _weightedElements;
        /// @}

        /// <summary> Default Constructor </summary>
        LinearPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The linear predictor to use when making the prediction. </param>
        LinearPredictorNode(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "LinearPredictorNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets an ObjectDescription for the type </summary>
        ///
        /// <returns> An ObjectDescription for the type </returns>
        static utilities::ObjectDescription GetTypeDescription();

        /// <summary> Gets an ObjectDescription for the object </summary>
        ///
        /// <returns> An ObjectDescription for the object </returns>
        virtual utilities::ObjectDescription GetDescription() const;

        /// <summary> Sets the internal state of the object according to the description passed in </summary>
        ///
        /// <param name="description"> The `ObjectDescription` to get state from </param>
        virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context);

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<double> _input;

        // Output
        model::OutputPort<double> _output;
        model::OutputPort<double> _weightedElements;

        // Linear predictor
        predictors::LinearPredictor _predictor;
    };

    /// <summary> Adds a linear predictor node to a model transformer. </summary>
    ///
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="predictor"> The linear predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    LinearPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor, model::ModelTransformer& transformer);
}
