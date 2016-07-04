////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (predictors_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TreePredictor.h"

// testing
#include "testing.h"

void TreePredictorTest()
{
    predictors::SingleInputThresholdRule rule{ 0 ,0 };
    std::vector<predictors::ConstantPredictor> edgePredictors { 1,2 };// { predictors::ConstantPredictor(1), predictors::ConstantPredictor(2) };
    predictors::SimpleTreePredictor::SplitInfo splitInfo{ rule, edgePredictors };

    predictors::SimpleTreePredictor::Leaf leaf{ 0, 0 };
    predictors::SimpleTreePredictor::SplitCandidate{ leaf, splitInfo };
    
    predictors::SimpleTreePredictor simpleTree;
    simpleTree.Split(splitInfo);
}

/// Runs all tests
///
int main()
{
    return 0;
}




