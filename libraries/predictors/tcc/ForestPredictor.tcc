////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestPredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictor.h"

// utilities
#include "Exception.h"

namespace predictors
{

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumInteriorNodes(size_t treeIndex) const
    {
        if (treeIndex >= _trees.size())
        {
            return 0;
        }
        else
        {
            return _trees[treeIndex].numInteriorNodes;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumEdges(size_t treeIndex) const
    {
        if (treeIndex >= _trees.size())
        {
            return 0;
        }
        else
        {
            return _trees[treeIndex].numEdges;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Compute(const RandomAccessVectorType& input) const
    {
        double output = 0.0;
        size_t numTrees = NumTrees();
        for (size_t i=0; i<numTrees; ++i)
        {
            output += Compute(input, i);
        }
        return output;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Compute(const RandomAccessVectorType& input, size_t treeIndex) const
    {
        if (treeIndex >= _trees.size())
        {
            return 0.0;
        }

        size_t nodeIndex = _trees[treeIndex].rootNodeIndex;
        double output = 0.0;

        do
        {
            const auto& interiorNode = _interiorNodes[nodeIndex];

            // which way do we go?
            int edgeIndex = interiorNode.splitRule.Compute(input);

            // check for early eject
            if (edgeIndex < 0)
            {
                break;
            }

            // add the edge prediction to output and follow the edge to the next node
            auto edge = interiorNode.outgoingEdges[edgeIndex];
            output += edge.predictor.Compute(input);
            nodeIndex = edge.targetNodeIndex;
        }
        while (nodeIndex != 0);

        return output;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    std::vector<bool> ForestPredictor<SplitRuleType, EdgePredictorType>::GetEdgePathIndicatorVector(const RandomAccessVectorType& input, size_t treeIndex) const
    {
        if (treeIndex >= _trees.size())
        {
            return std::vector<bool>(0);
        }

        size_t nodeIndex = 0;
        const auto& tree = _trees[treeIndex];
        std::vector<bool> pathIndicator(tree.numEdges);

        do
        {
            const auto& interiorNode = _interiorNodes[nodeIndex];

            // which way do we go?
            int edgeIndex = interiorNode.splitRule.Compute(input);

            // check for early eject
            if (edgeIndex < 0)
            {
                break;
            }
            // indicate the path in the vector and follow the edge to the next node
            auto edge = interiorNode.outgoingEdges[edgeIndex];
            pathIndicator[interiorNode.firstEdgeIndex + edgeIndex] = true;
            nodeIndex = edge.targetNodeIndex;
        }
        while (nodeIndex != 0);

        return pathIndicator;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictorInteriorNodeId ForestPredictor<SplitRuleType, EdgePredictorType>::AddTree(const SplitInfo& splitInfo)
    {
        size_t treeIndex = _trees.size();

        // add interior Node
        size_t interiorNodeIndex = AddInteriorNode(splitInfo, 0);

        // add new tree
        _trees.emplace_back({ interiorNodeIndex, 1, splitInfo.predictors.size() });

        // return ID of new root
        return{ treeIndex, interiorNodeIndex };
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictorInteriorNodeId ForestPredictor<SplitRuleType, EdgePredictorType>::SplitLeaf(const SplitInfo& splitInfo, ForestPredictorLeafId ForestPredictorLeafId)
    {   
        // check that the parent of the leaf exists
        if(leaf.interiorNodeIndex >= _interiorNodes.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - parent of leaf does not exist");
        }

        // check that this leaf exists
        if(leaf.leafIndex >= _interiorNodes[leaf.interiorNodeIndex].outgoingEdges.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - leaf does not exist");
        }

        // check that this node wasn't previously split
        auto& parentOutgoing = _interiorNodes[leaf.interiorNodeIndex].outgoingEdges[leaf.leafIndex].targetNodeIndex;
        if (parentOutgoing != 0)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - attempted to split a non-leaf");
        }

        // add interior Node
        size_t interiorNodeIndex = AddInteriorNode(splitInfo, _tree[treeIndex].numEdges);

        // update the parent about the new interior node
        parentOutgoing = interiorNodeIndex;

        // update tree
        size_t treeIndex = ForestPredictorLeafId.interiorNodeId.treeIndex;
        _tree[treeIndex].numInteriorNodes++;
        _tree[treeIndex].numEdges += splitInfo.predictors.size();

        // return ID of new interior node
        return{ treeIndex, interiorNodeIndex };
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::AddInteriorNode(const SplitInfo& splitInfo, size_t firstEdgeIndex)
    {
        size_t numEdges = splitInfo.predictors.size();

        // check correctness of splitInfo
        if (numEdges != splitInfo.splitRule.NumOutputs())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - number of split rule outputs doesn't match fan-out");
        }

        // get indices
        size_t interiorNodeIndex = _interiorNodes.size();

        // create the new interior node
        _interiorNodes.emplace_back(splitInfo, firstEdgeIndex);

        // increment global edge count
        _numEdges += numEdges;

        return interiorNodeIndex;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::Edge(const EdgePredictorType& predictor) : predictor(predictor), targetNodeIndex(0) 
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::InteriorNode(const SplitInfo& splitInfo, size_t firstEdgeIndex) : splitRule(splitInfo.splitRule), firstEdgeIndex(firstEdgeIndex)
    {
        std::copy(splitInfo.predictors.begin(), splitInfo.predictors.end(), std::back_inserter(outgoingEdges));
    }
}