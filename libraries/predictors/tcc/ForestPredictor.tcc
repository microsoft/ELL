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
    ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId::SplittableNodeId(size_t parentNodeIndex, size_t childPosition) : _isRoot(false), _parentNodeIndex(parentNodeIndex), _childPosition(childPosition)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::SplitAction::SplitAction(SplittableNodeId nodeId, SplitRuleType splitRule, std::vector<EdgePredictorType> edgePredictors) : _nodeId(std::move(nodeId)), _splitRule(std::move(splitRule)), _edgePredictors(std::move(edgePredictors))
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    bool ForestPredictor<SplitRuleType, EdgePredictorType>::IsTrivial() const
    {
        if(_trees.size() == 0 && _bias == 0.0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumInteriorNodes(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }

        auto const& interiorNode = _interiorNodes[interiorNodeIndex];
        size_t numInteriorNodes = 1;

        for(const auto& edge : interiorNode.outgoingEdges)
        {
            if(edge.targetNodeIndex > 0)
            {
                numInteriorNodes += NumInteriorNodes(edge.targetNodeIndex);
            }
        }

        return numInteriorNodes;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumEdges(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }

        auto const& interiorNode = _interiorNodes[interiorNodeIndex];
        size_t numEdges = interiorNode.outgoingEdges.size();

        for(const auto& edge : interiorNode.outgoingEdges)
        {
            if(edge.targetNodeIndex > 0)
            {
                numEdges += NumEdges(edge.targetNodeIndex);
            }
        }

        return numEdges;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Compute(const RandomAccessVectorType& input) const
    {
        double output = _bias;
        for (const auto& tree : _trees)
        {
            output += Compute(input, tree.rootIndex);
        }
        return output;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Compute(const RandomAccessVectorType& input, size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0.0;
        }

        size_t nodeIndex = interiorNodeIndex;
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
    std::vector<bool> ForestPredictor<SplitRuleType, EdgePredictorType>::GetEdgeIndicatorVector(const RandomAccessVectorType& input) const
    {
        std::vector<bool> edgeIndicator(_numEdges);
        for (const auto& tree : _trees)
        {
            SetEdgeIndicatorVector(input, edgeIndicator, tree.rootIndex);
        }
        return edgeIndicator;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    std::vector<bool> ForestPredictor<SplitRuleType, EdgePredictorType>::GetEdgeIndicatorVector(const RandomAccessVectorType& input, size_t interiorNodeIndex) const
    {
        std::vector<bool> edgeIndicator(_numEdges);
        SetEdgeIndicatorVector(input, edgeIndicator, interiorNodeIndex);
        return edgeIndicator;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumChildren(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }
        return _interiorNodes[interiorNodeIndex].outgoingEdges.size();
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    typename ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId ForestPredictor<SplitRuleType, EdgePredictorType>::GetChildId(size_t parentNodeIndex, size_t childPosition) const
    {
        // check that the parent exists
        if(parentNodeIndex >= _interiorNodes.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid identifier requested - parent does not exist");
        }

        // check that the splittable node exists
        if(childPosition >= _interiorNodes[parentNodeIndex].outgoingEdges.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid identifier requested - child does not exist");
        }

        return SplittableNodeId(parentNodeIndex, childPosition);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::Split(const SplitAction& splitAction)
    {
        if(splitAction._nodeId._isRoot)
        {
            // add interior Node
            size_t interiorNodeIndex = AddInteriorNode(splitAction);

            // add new tree
            _trees.push_back({ interiorNodeIndex });

            // return ID of new root
            return interiorNodeIndex;
        }
        else
        {
            // check that this node wasn't previously split
            auto& parentOutgoing = _interiorNodes[splitAction._nodeId._parentNodeIndex].outgoingEdges[splitAction._nodeId._childPosition].targetNodeIndex;
            if (parentOutgoing != 0)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - node previously split");
            }

            // add interior Node
            size_t interiorNodeIndex = AddInteriorNode(splitAction);

            // update the parent about the new interior node
            parentOutgoing = interiorNodeIndex;

            // return ID of new interior node
            return interiorNodeIndex;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SetBias(double value)
    {
        _bias = value;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    template<typename RandomAccessVectorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SetEdgeIndicatorVector(const RandomAccessVectorType& input, std::vector<bool>& output, size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return;
        }

        size_t nodeIndex = interiorNodeIndex;

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

            // indicate the edge in the vector and follow the edge to the next node
            auto edge = interiorNode.outgoingEdges[edgeIndex];
            output[interiorNode.firstEdgeIndex + edgeIndex] = true;
            nodeIndex = edge.targetNodeIndex;
        }
        while (nodeIndex != 0);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::AddInteriorNode(const SplitAction& splitAction)
    {
        size_t numEdges = splitAction._edgePredictors.size();

        // check correctness of splitAction
        if (numEdges != splitAction._splitRule.NumOutputs())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - number of split rule outputs doesn't match fan-out");
        }

        // get indices
        size_t interiorNodeIndex = _interiorNodes.size();

        // create the new interior node
        _interiorNodes.emplace_back(splitAction, _numEdges);

        // increment global edge count
        _numEdges += numEdges;

        return interiorNodeIndex;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::Edge(const EdgePredictorType& predictor) : predictor(predictor), targetNodeIndex(0) 
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::InteriorNode(const SplitAction& splitAction, size_t firstEdgeIndex) : splitRule(splitAction._splitRule), firstEdgeIndex(firstEdgeIndex)
    {
        std::copy(splitAction._edgePredictors.begin(), splitAction._edgePredictors.end(), std::back_inserter(outgoingEdges));
    }

    //
    // debugging code
    //

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId::Print(std::ostream & os) const
    {
        if (_isRoot)
        {
            os << "root";
        }
        else
        {
            os << "child " << _childPosition << " of node " << _parentNodeIndex;
        }
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SplitAction::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "action = split ";
        _nodeId.Print(os);
        os << "\n";

        os << std::string(tabs * 4, ' ') << "rule:\n";
        _splitRule.PrintLine(os, tabs + 1);

        os << std::string(tabs * 4, ' ') << "edge predictors:\n";
        for(const auto& predictor : _edgePredictors)
        {
            predictor.PrintLine(os, tabs + 1);
        }
    }

    //
    // debugging members
    // 

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::PrintLine(std::ostream& os, size_t tabs) const
    {

    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::PrintLine(std::ostream & os, size_t tabs) const
    {
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::PrintLine(std::ostream & os, size_t tabs) const
    {
    }
}