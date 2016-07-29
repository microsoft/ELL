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

    SingleElementThresholdRuleSubModelOutputs BuildSubModel(const predictors::SingleElementThresholdRule& rule, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        // get the element used in the split rule // TODO - this is a broken way to get the element. 
        auto elementAsRange = outputPortElements.GetElement(rule.GetElementIndex());
        model::OutputPortElements<double> element;
        element.AddRange(elementAsRange);

        // get the threshold.
        auto thresholdNode = model.AddNode<ConstantNode<double>>(rule.GetThreshold());

        // create a lessThanOrEqual predicate that implements the split rule
        auto binaryPredicateNode = model.AddNode<BinaryPredicateNode<double>>(element, thresholdNode->output, BinaryPredicateNode<double>::PredicateType::greater);
        return{ binaryPredicateNode->output };
    }
}