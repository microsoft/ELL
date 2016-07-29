////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SubModelBuilders.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SubModelBuilders.h"
#include "ConstantNode.h"
#include "BinaryPredicateNode.h"

// model
#include "OutputPortElements.h"

namespace nodes
{
    ConstantPredictorSubModelOutputs BuildSubModel(const predictors::ConstantPredictor& predictor, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        auto constantNode = model.AddNode<ConstantNode<double>>(predictor.GetValue());
        return{ constantNode->output };
    }

    SingleInputThresholdRuleSubModelOutputs BuildSubModel(const predictors::SingleInputThresholdRule& rule, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        // get the element index
        size_t elementIndex = rule.GetElementIndex();
        //model::OutputPortElements<double> element = { outputPortElements, elementIndex };
        model::OutputPortElements<double> element;

        // get the threshold .
        auto thresholdNode = model.AddNode<ConstantNode<double>>(rule.GetThreshold());
        auto binaryPredicateNode = model.AddNode<BinaryPredicateNode<double>>(element, thresholdNode->output, BinaryPredicateNode<double>::PredicateType::lessThanOrEqual);

        return{ binaryPredicateNode->output };
    }

    // add BinaryPredicateNode with operation less than
    // SingleInput --> SingleElement
    // TreeRoot ---> Root
}