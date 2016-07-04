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
    size_t TreePredictor<SplitRuleType, EdgePredictorType>::Split(const SplitCandidate& splitCandidate)
    {
        auto splitInfo = splitCandidate.splitInfo;
    
        // sanity check - check correctness of splitInfo
        if (splitInfo.predictors.size() != splitInfo.splitRule.NumOutputs())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - number of split rule outputs doesn't match fan-out");
        }

        // get the index of the new interior node
        size_t newInteriorNodeIndex = _interiorNodes.size();

        // create the new interior node
        _interiorNodes.emplace_back(splitInfo);

        // increment edge count
        _numEdges += splitInfo.predictors.size();

        // sanity check - check that split refers to a leaf
        auto leaf = splitCandidate.leaf;
        auto& parentOutgoing = _interiorNodes[leaf.interiorNodeIndex].outgoingEdges[leaf.leafIndex].targetNodeIndex;
        if (parentOutgoing != 0)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - attempted to split a non-leaf");
        }

        // update the parent about the new interior node
        parentOutgoing = newInteriorNodeIndex;
        
        // return index of new interior node
        return newInteriorNodeIndex;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    TreePredictor<SplitRuleType, EdgePredictorType>::Edge::Edge(const EdgePredictorType& predictor) : predictor(predictor), targetNodeIndex(0) 
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    TreePredictor<SplitRuleType, EdgePredictorType>::InteriorNode::InteriorNode(const SplitInfo& splitInfo) : splitRule(splitInfo.splitRule)
    {
        std::copy(splitInfo.predictors.begin(), splitInfo.predictors.end(), std::back_inserter(outgoingEdges));
    }
}