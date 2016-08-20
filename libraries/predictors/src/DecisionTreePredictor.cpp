////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DecisionTreePredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DecisionTreePredictor.h"

// stl
#include <stdexcept>
#include <cassert>

namespace predictors
{
    DecisionTreePredictor::Node::Node(double outputValue) : _outputValue(outputValue)
    {}

    double DecisionTreePredictor::Node::Predict(const dataset::DoubleDataVector& dataVector) const
    {
        double output = _outputValue;
        if (_interiorNode != nullptr)
        {
            output += _interiorNode->Predict(dataVector);
        }
        return output;
    }

    DecisionTreePredictor::InteriorNode& DecisionTreePredictor::Node::Split(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue)
    {
        // confirm that this is a leaf
        assert(IsLeaf());

        _interiorNode = std::make_unique<InteriorNode>(std::move(splitRule), Node(negativeEdgeOutputValue), Node(positiveEdgeOutputValue));
        return *_interiorNode;
    }

    DecisionTreePredictor::InteriorNode::InteriorNode(SplitRule splitRule, Node negativeChild, Node positiveChild) :
        _splitRule(splitRule), _negativeChild(std::move(negativeChild)), _positiveChild(std::move(positiveChild))
    {}

    uint64_t DecisionTreePredictor::InteriorNode::NumInteriorNodesInSubtree() const
    {
        uint64_t num = 1;

        if(_negativeChild._interiorNode != nullptr)
        {
            num += _negativeChild._interiorNode->NumInteriorNodesInSubtree();
        }

        if(_positiveChild._interiorNode != nullptr)
        {
            num += _positiveChild._interiorNode->NumInteriorNodesInSubtree();
        }

        return num;
    }

    double DecisionTreePredictor::InteriorNode::Predict(const dataset::DoubleDataVector& dataVector) const
    {
        if (dataVector[_splitRule.featureIndex] <= _splitRule.threshold)
        {
            return _negativeChild.Predict(dataVector);
        }
        else
        {
            return _positiveChild.Predict(dataVector);
        }
    }

    DecisionTreePredictor::DecisionTreePredictor(double rootOutputValue) : _root(rootOutputValue)
    {}

    uint64_t DecisionTreePredictor::NumNodes() const
    {
        return 2*NumInteriorNodes()+1;
    }

    uint64_t DecisionTreePredictor::NumInteriorNodes() const
    {
        if(_root._interiorNode == nullptr)
        {
            return 0;
        }
        return _root._interiorNode->NumInteriorNodesInSubtree();
    }

    double DecisionTreePredictor::Predict(const dataset::IDataVector& dataVector) const
    {
        dataset::DoubleDataVector denseDataVector(dataVector.ToArray());
        return _root.Predict(denseDataVector);
    }

    void predictors::DecisionTreePredictor::BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const
    {
        if (interiorNodePtr == nullptr)
        {
            return;
        }

        const auto& splitRule = interiorNodePtr->GetSplitRule();
        auto splitRuleCoordinate = inputCoordinates[splitRule.featureIndex];
        auto negativeThreshold = -(splitRule.threshold);

        flatTree.splitRuleCoordinates.AddCoordinate(splitRuleCoordinate);
        flatTree.negativeThresholds.push_back(negativeThreshold);

        flatTree.nonRootOutputValues.push_back(interiorNodePtr->GetNegativeChild()._outputValue);
        flatTree.nonRootOutputValues.push_back(interiorNodePtr->GetPositiveChild()._outputValue);

        if (interiorNodePtr->GetNegativeChild().IsLeaf())
        {
            flatTree.edgeToInteriorNode.push_back(0);
            flatTree.edgeToInteriorNode.push_back(0);
        }
        else
        {
            auto negativeChildIndex = flatTree.splitRuleCoordinates.Size();
            flatTree.edgeToInteriorNode.push_back(negativeChildIndex);

            // as a placeholder, set edgeToInteriorNode[positiveEdgeIndex] = 0
            auto positiveEdgeIndex = flatTree.edgeToInteriorNode.size();
            flatTree.edgeToInteriorNode.push_back(0);

            // recurse (DFS) to the negative side
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetNegativeChild()._interiorNode.get());
            
            // set edgeToInteriorNode[positiveEdgeIndex] to its correct value
            auto positiveChildIndex = flatTree.splitRuleCoordinates.Size();
            flatTree.edgeToInteriorNode[positiveEdgeIndex] = positiveChildIndex;
            
            // recurse (DFS) to the positive side
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetPositiveChild()._interiorNode.get());
        }
    }
}
