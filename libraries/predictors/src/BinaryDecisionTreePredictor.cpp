////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryDecisionTreePredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryDecisionTreePredictor.h"

namespace predictors
{
    size_t BinaryDecisionTreePredictor::Rule::operator()(const dataset::DoubleDataVector& dataVector) const
    {
        return dataVector[_inputIndex] <= _threshold ? 0 : 1;
    }

    double BinaryDecisionTreePredictor::Predict(const dataset::DoubleDataVector& input) const
    {
        // handle empty trees
        if (_interiorNodes.size() == 0)
        {
            return 0.0;
        }

        size_t nodeIndex = 0;
        double prediction = 0.0;

        do
        {
            // which way do we go?
            size_t ruleEvaluation = _interiorNodes[nodeIndex].rule(input);
            
            // follow the edge and add its weight to the output
            auto edge = _interiorNodes[nodeIndex].edgeData[ruleEvaluation];
            nodeIndex = edge.targetNodeIndex;
            prediction += edge.weight;
        } 
        while (nodeIndex != 0);

        return prediction;
    }

    std::vector<bool> BinaryDecisionTreePredictor::GetEdgePathIndicator(const dataset::DoubleDataVector& input) const
    {
        // handle empty trees
        if (_interiorNodes.size() == 0)
        {
            return std::vector<bool>(0);
        }

        size_t nodeIndex = 0;
        std::vector<bool> pathIndicator(NumEdges());

        do
        {
            // which way do we go?
            size_t ruleEvaluation = _interiorNodes[nodeIndex].rule(input);

            // follow the edge and mark it in the path indicator vector
            auto edge = _interiorNodes[nodeIndex].edgeData[ruleEvaluation];
            nodeIndex = edge.targetNodeIndex;
            pathIndicator[nodeIndex] = true;
        } 
        while (nodeIndex != 0);

        return pathIndicator;
    }

    size_t BinaryDecisionTreePredictor::Split(const SplitInfo& splitInfo)
    {
        // get the index of the new interior node
        size_t interiorNodeIndex = _interiorNodes.size();

        // create the new interior node
        _interiorNodes.push_back({ splitInfo.rule,{ splitInfo.edgeWeights[0], splitInfo.edgeWeights[1] } });

        // connect the new interior node to its parent
        _interiorNodes[splitInfo.leaf.interiorNodeIndex].edgeData[splitInfo.leaf.leafIndex].targetNodeIndex = interiorNodeIndex;
        
        return interiorNodeIndex;
    }

    BinaryDecisionTreePredictor::EdgeData::EdgeData(double w) : weight(w), targetNodeIndex(0)
    {}
}