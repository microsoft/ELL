////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreePredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TreePredictor.h"

namespace predictors
{
    template<typename SplitRuleType>
    template<typename RandomAccessVectorType>
    double TreePredictor<SplitRuleType>::Compute(const RandomAccessVectorType& input) const
    {
        // handle empty trees
        if (_interiorNodes.size() == 0)
        {
            return 0.0;
        }

        size_t nodeIndex = 0;
        double output = 0.0;

        do
        {
            // which way do we go?
            size_t edgeIndex = _interiorNodes[nodeIndex].splitRule.Compute(input);
            
            // follow the edge and add its weight to the output
            auto edge = _interiorNodes[nodeIndex].edgeData[edgeIndex];
            nodeIndex = edge.targetNodeIndex;
            output += edge.weight;
        } 
        while (nodeIndex != 0);

        return output;
    }

    //template<typename SplitRuleType>
    //template<typename RandomAccessVectorType>
    //std::vector<bool> TreePredictor::GetEdgePathIndicator(const dataset::DoubleDataVector& input) const
    //{
    //    // handle empty trees
    //    if (_interiorNodes.size() == 0)
    //    {
    //        return std::vector<bool>(0);
    //    }

    //    size_t nodeIndex = 0;
    //    std::vector<bool> pathIndicator(NumEdges());

    //    do
    //    {
    //        // which way do we go?
    //        size_t ruleEvaluation = _interiorNodes[nodeIndex].rule(input);

    //        // follow the edge and mark it in the path indicator vector
    //        auto edge = _interiorNodes[nodeIndex].edgeData[ruleEvaluation];
    //        nodeIndex = edge.targetNodeIndex;
    //        pathIndicator[nodeIndex] = true;
    //    } 
    //    while (nodeIndex != 0);

    //    return pathIndicator;
    //}

    //size_t TreePredictor::Split(const SplitInfo& splitInfo)
    //{
    //    // get the index of the new interior node
    //    size_t interiorNodeIndex = _interiorNodes.size();

    //    // create the new interior node
    //    _interiorNodes.push_back({ splitInfo.rule,{ splitInfo.edgeWeights[0], splitInfo.edgeWeights[1] } });

    //    // connect the new interior node to its parent
    //    _interiorNodes[splitInfo.leaf.interiorNodeIndex].edgeData[splitInfo.leaf.leafIndex].targetNodeIndex = interiorNodeIndex;
    //    
    //    return interiorNodeIndex;
    //}

    //TreePredictor::EdgeData::EdgeData(double w) : weight(w), targetNodeIndex(0)
    //{}
}