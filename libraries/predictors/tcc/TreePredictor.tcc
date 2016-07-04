////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreePredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TreePredictor.h"

// utilities
#include "Exception.h"

namespace predictors
{
    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    double TreePredictor<SplitRuleType, EdgePredictorType>::Compute(const RandomAccessVectorType& input) const
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
            int edgeIndex = _interiorNodes[nodeIndex].splitRule.Compute(input);

            // check for early eject
            if (edgeIndex < 0)
            {
                break;
            }

            // follow the edge and add its weight to the output
            auto edge = _interiorNodes[nodeIndex].outgoingEdges[edgeIndex];
            nodeIndex = edge.targetNodeIndex;
            output += edge.predictor.Compute(input);
        } 
        while (nodeIndex != 0);

        return output;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    std::vector<bool> TreePredictor<SplitRuleType, EdgePredictorType>::GetEdgePathIndicatorVector(const RandomAccessVectorType& input) const
    {
        // handle empty trees
        if (_interiorNodes.size() == 0)
        {
            return std::vector<bool>(0);
        }

        size_t nodeIndex = 0;
        std::vector<bool> pathIndicator(_numEdges);

        do
        {
            // which way do we go?
            int edgeIndex = _interiorNodes[nodeIndex].splitRule.Compute(input);

            // check for early eject
            if (edgeIndex < 0)
            {
                break;
            }
            // follow the edge and add its weight to the output
            auto edge = _interiorNodes[nodeIndex].outgoingEdges[edgeIndex];
            nodeIndex = edge.targetNodeIndex;
            pathIndicator[nodeIndex] = true;
        } 
        while (nodeIndex != 0);

        return pathIndicator;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t TreePredictor<SplitRuleType, EdgePredictorType>::Split(const SplitInfo& splitInfo)
    {
    
        // check correctness of splitInfo
        if (splitInfo.predictors.size() != splitInfo.splitRule.NumOutputs())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - number of split rule outputs doesn't match fan-out");
        }

        // get the index of the new interior node
        size_t interiorNodeIndex = _interiorNodes.size();

        auto interiorNode = InteriorNodeData(splitInfo); 

        //// create the new interior node
        //_interiorNodes.push_back();

        //// connect the new interior node to its parent
        //_interiorNodes[splitInfo.leaf.interiorNodeIndex].outgoingEdges[splitInfo.leaf.leafIndex].targetNodeIndex = interiorNodeIndex;
        
        return interiorNodeIndex;
    }

    //template<typename SplitRuleType, typename EdgePredictorType>
    //TreePredictor<SplitRuleType, EdgePredictorType>::EdgeData::EdgeData(const EdgePredictorType& predictor) : predictor(predictor), targetNodeIndex(0)
    //{}

    //template<typename SplitRuleType, typename EdgePredictorType>
    //TreePredictor<SplitRuleType, EdgePredictorType>::InteriorNodeData::InteriorNodeData(const SplitRuleType& splitRule, const std::array<EdgePredictorType, FanOut>& edgePredictors) : splitRule(splitRule)
    //{
    //    // TODO
    //}
}