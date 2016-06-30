////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryDecisionTreePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace predictors
{
    /// <summary> Implements a proper binary decision tree ('proper binary' means that each interior
    /// node has exactly two children). Each edge is assigned a weight and the output of the tree is
    /// the sum along the path from the tree root to a leaf. Note that assigning weights to edges is
    /// equivalent to assigning weights to all nodes other than the root. </summary>
    class BinaryDecisionTreePredictor
    {
    public:



        size_t NumInteriorNodes() const { _interiorNodes.size(); }



    private:

        struct EdgeData
        {
            double weight = 0.0;
            size_t targetNode = 0;
        };

        struct InteriorNodeData
        {
            EdgeData falseEdgeData;
            EdgeData trueEdgeData;
        };

        std::vector<InteriorNodeData> _interiorNodes;
    };
}