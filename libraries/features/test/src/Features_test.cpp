////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Features_test.cpp (features_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FeatureSet.h"
#include "Feature.h"
#include "InputFeature.h"
#include "MagnitudeFeature.h"

// datset
#include "DenseDataVector.h"

// linear
#include "DoubleVector.h"

// layers
#include "Model.h"
#include "Map.h"

// utilities
#include "StlIndexValueIterator.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

using namespace features;

double VecMagnitude(const std::vector<double>& vec)
{
    double sumSq = 0;
    for(const auto& x: vec) sumSq += (x*x);
    
    return std::sqrt(sumSq);
}

void TestMagFeatureCompute()
{
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(3);
    auto magnitudeFeature = features.CreateFeature<MagnitudeFeature>(inputFeature);
    features.SetOutputFeature(magnitudeFeature);
    features.Reset();

    std::vector<double> data = { 1,2,3 };
    double output = 0;
    bool hasOutput = features.ProcessInputData(data);
    if (hasOutput)
    {
        auto out = features.GetOutput();
        output = out[0];
    }
    else
    {
        // TODO: maybe call ProcessTest Assert? Maybe even add testing::FailTest(msg) for situations like this?
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    double expectedOutput = VecMagnitude(data);
    testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(output, expectedOutput));
}

void TestMagFeatureModel()
{
    // Set up feature set
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(3);
    auto magnitudeFeature = features.CreateFeature<MagnitudeFeature>(inputFeature);
    features.SetOutputFeature(magnitudeFeature);

    // create model
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 3); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);
    std::cout << "Output coordinates: " << outputCoordinates << std::endl;

    model.Save(std::cout);
    std::cout << std::endl;
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    std::vector<double> data = { 1,2,3 };
    dataset::DoubleDataVector dataVector(data);
    auto outputVec = map.Compute(data);
    return;
    
    // double output;
    // if (outputVec.size() > 0)
    // {
    //     output = outputVec[0];
    // }
    // else
    // {
    //     // TODO: maybe rename ProcessTest to Assert? Maybe even add testing::FailTest(msg) for situations like this?
    //     testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    // }
    // std::cout << "Output vec: ";
    // for(const auto& x: outputVec) std::cout << x << "  ";
    // std::cout << std::endl;
    // std::cout << "Output: " << output << std::endl;
    // double expectedOutput = VecMagnitude(data);
    // testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(output, expectedOutput));
        
}
