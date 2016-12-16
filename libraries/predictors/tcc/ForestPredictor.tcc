////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestPredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictor.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace predictors
{
    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId::SplittableNodeId(size_t parentNodeIndex, size_t childPosition)
        : _isRoot(false), _parentNodeIndex(parentNodeIndex), _childPosition(childPosition)
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::SplitAction::SplitAction(SplittableNodeId nodeId, SplitRuleType _splitRule, std::vector<EdgePredictorType> edgePredictors)
        : _nodeId(std::move(nodeId)), _splitRule(std::move(_splitRule)), _edgePredictors(std::move(edgePredictors))
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::Edge(const EdgePredictorType& predictor)
        : _predictor(predictor), _targetNodeIndex(0)
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::SetTargetNodeIndex(size_t targetNodeIndex)
    {
        _targetNodeIndex = targetNodeIndex;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    bool ForestPredictor<SplitRuleType, EdgePredictorType>::IsTrivial() const
    {
        if (_rootIndices.size() == 0 && _bias == 0.0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumInteriorNodes(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }

        auto const& interiorNode = _interiorNodes[interiorNodeIndex];
        size_t numInteriorNodes = 1;

        for (const auto& edge : interiorNode._outgoingEdges)
        {
            if (edge.IsTargetInterior())
            {
                numInteriorNodes += NumInteriorNodes(edge.GetTargetNodeIndex());
            }
        }

        return numInteriorNodes;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumEdges(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }

        auto const& interiorNode = _interiorNodes[interiorNodeIndex];
        size_t numEdges = interiorNode._outgoingEdges.size();

        for (const auto& edge : interiorNode._outgoingEdges)
        {
            if (edge.IsTargetInterior())
            {
                numEdges += NumEdges(edge.GetTargetNodeIndex());
            }
        }

        return numEdges;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Predict(const DataVectorType& input) const
    {
        double output = _bias;
        for (auto treeRootIndex : _rootIndices)
        {
            output += Predict(input, treeRootIndex);
        }
        return output;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    double ForestPredictor<SplitRuleType, EdgePredictorType>::Predict(const DataVectorType& input, size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0.0;
        }

        double output = 0.0;

        VisitEdgePathToLeaf(input, interiorNodeIndex, [&](const InteriorNode& interiorNode, size_t edgePosition) { output += interiorNode._outgoingEdges[edgePosition]._predictor.Predict(input); });

        return output;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    std::vector<bool> ForestPredictor<SplitRuleType, EdgePredictorType>::GetEdgeIndicatorVector(const DataVectorType& input) const
    {
        std::vector<bool> edgeIndicator(_numEdges);
        for (auto treeRootIndex : _rootIndices)
        {
            SetEdgeIndicatorVector(input, edgeIndicator, treeRootIndex);
        }
        return edgeIndicator;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    std::vector<bool> ForestPredictor<SplitRuleType, EdgePredictorType>::GetEdgeIndicatorVector(const DataVectorType& input, size_t interiorNodeIndex) const
    {
        std::vector<bool> edgeIndicator(_numEdges);
        SetEdgeIndicatorVector(input, edgeIndicator, interiorNodeIndex);
        return edgeIndicator;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::NumChildren(size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return 0;
        }
        return _interiorNodes[interiorNodeIndex]._outgoingEdges.size();
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    typename ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId ForestPredictor<SplitRuleType, EdgePredictorType>::GetChildId(size_t parentNodeIndex, size_t childPosition) const
    {
        // check that the parent exists
        if (parentNodeIndex >= _interiorNodes.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid identifier requested - parent does not exist");
        }

        // check that the splittable node exists
        if (childPosition >= _interiorNodes[parentNodeIndex]._outgoingEdges.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid identifier requested - child does not exist");
        }

        return SplittableNodeId(parentNodeIndex, childPosition);
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    size_t ForestPredictor<SplitRuleType, EdgePredictorType>::Split(const SplitAction& splitAction)
    {
        if (splitAction._nodeId._isRoot)
        {
            // add interior Node
            size_t interiorNodeIndex = AddInteriorNode(splitAction);

            // add new tree
            _rootIndices.push_back(interiorNodeIndex);

            // return ID of new root
            return interiorNodeIndex;
        }
        else
        {
            // check that this node wasn't previously split
            auto& incomingEdge = _interiorNodes[splitAction._nodeId._parentNodeIndex]._outgoingEdges[splitAction._nodeId._childPosition];
            if (incomingEdge.IsTargetInterior())
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "invalid split in decision tree - node previously split");
            }

            // add interior Node
            size_t interiorNodeIndex = AddInteriorNode(splitAction);

            // update the parent about the new interior node
            incomingEdge.SetTargetNodeIndex(interiorNodeIndex);

            // return ID of new interior node
            return interiorNodeIndex;
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::AddToBias(double value)
    {
        _bias += value;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["interiorNodes"] << _interiorNodes;
        archiver["rootIndices"] << _rootIndices;
        archiver["bias"] << _bias;
        archiver["numEdges"] << _numEdges;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["interiorNodes"] >> _interiorNodes;
        archiver["rootIndices"] >> _rootIndices;
        archiver["bias"] >> _bias;
        archiver["numEdges"] >> _numEdges;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SetEdgeIndicatorVector(const DataVectorType& input, std::vector<bool>& output, size_t interiorNodeIndex) const
    {
        if (interiorNodeIndex >= _interiorNodes.size())
        {
            return;
        }
        VisitEdgePathToLeaf(input, interiorNodeIndex, [&output](const InteriorNode& interiorNode, size_t edgePosition) { output[interiorNode._firstEdgeIndex + edgePosition] = true; });
    }

    template <typename SplitRuleType, typename EdgePredictorType>
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
        InteriorNode interiorNode(splitAction, _numEdges);
        _interiorNodes.push_back(std::move(interiorNode));

        // increment global edge count
        _numEdges += numEdges;

        return interiorNodeIndex;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::VisitEdgePathToLeaf(const DataVectorType& input, size_t interiorNodeIndex, std::function<void(const InteriorNode&, size_t edgePosition)> operation) const
    {
        size_t nodeIndex = interiorNodeIndex;

        do
        {
            const auto& interiorNode = _interiorNodes[nodeIndex];

            // which way do we go?
            int edgePosition = static_cast<int>(interiorNode._splitRule.Predict(input));

            // check for early eject
            if (edgePosition < 0)
            {
                break;
            }

            // apply the operation
            operation(interiorNode, edgePosition);

            //follow the edge to the next node
            const auto& edge = interiorNode._outgoingEdges[edgePosition];
            nodeIndex = edge.GetTargetNodeIndex();
        } while (nodeIndex != 0);
    }

    //
    // InteriorNode
    //
    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::InteriorNode(const SplitAction& splitAction, size_t _firstEdgeIndex)
        : _splitRule(splitAction._splitRule), _firstEdgeIndex(_firstEdgeIndex)
    {
        std::copy(splitAction._edgePredictors.begin(), splitAction._edgePredictors.end(), std::back_inserter(_outgoingEdges));
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["splitRule"] << _splitRule;
        archiver["outgoingEdges"] << _outgoingEdges;
        archiver["firstEdgeIndex"] << _firstEdgeIndex;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["splitRule"] >> _splitRule;
        archiver["outgoingEdges"] >> _outgoingEdges;
        archiver["firstEdgeIndex"] >> _firstEdgeIndex;
    }

    //
    // debugging code
    //

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SplittableNodeId::Print(std::ostream& os) const
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

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::SplitAction::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "action = split ";
        _nodeId.Print(os);
        os << "\n";

        os << std::string(tabs * 4, ' ') << "rule:\n";
        _splitRule.PrintLine(os, tabs + 1);

        os << std::string(tabs * 4, ' ') << "edge predictors:\n";
        for (const auto& predictor : _edgePredictors)
        {
            predictor.PrintLine(os, tabs + 1);
        }
    }

    //
    // debugging members
    //

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "Forest Predictor: bias = " << _bias << "\n";
        for (const auto& interiorNode : _interiorNodes)
        {
            interiorNode.PrintLine(os, tabs + 1);
        }
        for (auto treeRootIndex : _rootIndices)
        {
            os << std::string(tabs * 4, ' ') << "Tree: root index = " << treeRootIndex << "\n";
        }
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::InteriorNode::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "InteriorNode:\n";
        _splitRule.PrintLine(os, tabs + 1);
        for (const auto& edge : _outgoingEdges)
        {
            edge.PrintLine(os, tabs + 1);
        }
    }
    //
    // Edge
    //
    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "Edge:\n";
        _predictor.PrintLine(os, tabs + 1);
        os << std::string(tabs * 4, ' ') << "Target node index = " << _targetNodeIndex << "\n";
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    bool ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::IsTargetInterior() const
    {
        return _targetNodeIndex == 0 ? false : true;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["predictor"] << _predictor;
        archiver["targetNodeIndex"] << _targetNodeIndex;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictor<SplitRuleType, EdgePredictorType>::Edge::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["predictor"] >> _predictor;
        archiver["targetNodeIndex"] >> _targetNodeIndex;
    }
}
}
