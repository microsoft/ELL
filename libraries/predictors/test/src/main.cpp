////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (predictors_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictor.h"

// testing
#include "testing.h"

// linear
#include "DoubleVector.h"

void TreePredictorTest()
{
    // create an empty tree
    //predictors::SimpleTreePredictor simpleTree;

    //// split rule: compare feature 3 to threshold -0.2; edge outputs: less than = -1.1, greater than = 4.4  
    //predictors::SimpleTreePredictor::SplitInfo splitInfo{ predictors::SingleInputThresholdRule{ 3, -0.2 }, std::vector<predictors::ConstantPredictor>{ -1.1, 4.4 } };

    //// the leaf in question: child 1 of interior node 0 (the root)
    //predictors::SimpleTreePredictor::Leaf leaf{ 0, 1 };

    //// perform the split
    //simpleTree.Split(leaf, splitInfo);

    //// create an feature vector
    //linear::DoubleVector x({1,2,3,4,5});
    //auto output = simpleTree.Compute(x);
    //auto pathIndicator = simpleTree.GetEdgePathIndicatorVector(x);
}

/// Runs all tests
///
int main()
{
    return 0;
}




