////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// predictors
#include "LinearPredictor.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a linear predictor. </summary>
    template <typename ElementType>
    class LinearPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* weightedElementsPortName = "weightedElements";
        const model::InputPort<ElementType>& input = _input;
        const model::OutputPort<ElementType>& output = _output;
        const model::OutputPort<ElementType>& weightedElements = _weightedElements;
        /// @}

        using LinearPredictorType = typename predictors::LinearPredictor<ElementType>;

        /// <summary> Default Constructor </summary>
        LinearPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The linear predictor to use when making the prediction. </param>
        LinearPredictorNode(const model::PortElements<ElementType>& input, const LinearPredictorType& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("LinearPredictorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Inputs
        model::InputPort<ElementType> _input;

        // Output
        model::OutputPort<ElementType> _output;
        model::OutputPort<ElementType> _weightedElements;

        // Linear predictor
        LinearPredictorType _predictor;
    };

    /// <summary> Adds a linear predictor node to a model transformer. </summary>
    ///
    /// <typeparam name="ElementType"> The fundamental type used by this predictor. </typeparam>
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="predictor"> The linear predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>    
    template <typename ElementType>
    LinearPredictorNode<ElementType>* AddNodeToModelTransformer(const model::PortElements<ElementType>& input, const predictors::LinearPredictor<ElementType>& predictor, model::ModelTransformer& transformer);
}
}

#include "../tcc/LinearPredictorNode.tcc"
