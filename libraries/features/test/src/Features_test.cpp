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
#include "MeanFeature.h"
#include "IncrementalMeanFeature.h"
#include "VarianceFeature.h"
#include "IncrementalVarianceFeature.h"

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

//
// Helpers
//
namespace
{
    double VectorMagnitude(const std::vector<double>& vec)
    {
        double sumSq = 0.0;
        for(const auto& x: vec) sumSq += (x*x);
        
        return std::sqrt(sumSq);
    }

    double VectorMean(const std::vector<double>& vec)
    {
        double sum = 0.0;
        for(const auto& x: vec) sum += x;
        
        return sum / vec.size();
    }

    double VectorVariance(const std::vector<double>& vec, double mean)
    {
        if(vec.size() == 0) return 0.0;
        double var = 0.0;
        for (auto x : vec)
        {
            double diff = x - mean;
            var += diff*diff;
        }
        return var / vec.size();
    }

    std::ostream& operator<<(std::ostream& out, const std::vector<double> array)
    {
        out << "[";
        for(auto x: array) out << x << "  ";
        out << "]";
        return out;
    }
}

//
// Test functions
//
void TestMagnitudeFeature()
{
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(3);
    auto magnitudeFeature = features.CreateFeature<MagnitudeFeature>(inputFeature);
    features.SetOutputFeature(magnitudeFeature);
    features.Reset();

    std::vector<std::vector<double>> data = {{ 1,2,3 }};
    double expectedOutput = VectorMagnitude(data.back());

    // 1) Test Compute()
    std::vector<double> outputVec;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }

    if (hasOutput)
    {
        outputVec = static_cast<std::vector<double>>(features.GetOutput());
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(outputVec[0], expectedOutput));

    // 2) Create Model / Map and test it
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 3); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);

    std::cout << "Magnitude feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;

    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    if (outputVec.size() == 0)
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }
    testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestMeanFeature()
{
    FeatureSet features;
    const int windowSize = 4;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    auto meanFeature = features.CreateFeature<MeanFeature>(inputFeature, windowSize);
    features.SetOutputFeature(meanFeature);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 

    // 1) Test Compute()
    std::vector<double> outputVec;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }
    
    if (hasOutput)
    {
        outputVec = features.GetOutput();
    }
    else
    {
        testing::ProcessTest("Error: no output from MeanFeature", false); 
    }

    testing::ProcessTest("Testing MeanFeature::Compute", testing::IsEqual(outputVec[0], expectedOutput));
    
    // 2) Create Model / Map and test it
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);

    std::cout << "Mean feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    if (outputVec.size() == 0)
    {
        testing::ProcessTest("Error: no output from MeanFeature's Map", false); 
    }
    testing::ProcessTest("Testing MeanFeature::AddToModel", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestIncrementalMeanFeature()
{
    FeatureSet features;
    const int windowSize = 4;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    auto meanFeature = features.CreateFeature<IncrementalMeanFeature>(inputFeature, windowSize);
    features.SetOutputFeature(meanFeature);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 

    // 1) Test Compute()
    std::vector<double> outputVec;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }
    
    if (hasOutput)
    {
        outputVec = features.GetOutput();
    }
    else
    {
        testing::ProcessTest("Error: no output from IncrementalMeanFeature", false); 
    }

    testing::ProcessTest("Testing IncrementalMeanFeature::Compute", testing::IsEqual(outputVec[0], expectedOutput));
        
    // 2) Create Model / Map and test it
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);

    std::cout << "Incremental mean feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
        
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    if (outputVec.size() == 0)
    {
        testing::ProcessTest("Error: no output from IncrementalMeanFeature's Map", false); 
    }
    
    std::cout << "output: " << outputVec[0] << ", expected: " << expectedOutput << std::endl;
    
    testing::ProcessTest("Testing IncrementalMeanFeature::AddToModel", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestVarianceFeature()
{
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto varianceFeature = features.CreateFeature<VarianceFeature>(inputFeature, windowSize);
    features.SetOutputFeature(varianceFeature);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double mean = VectorMean({7.0, 8.0, 9.0, 10.0});
    double expectedOutput = VectorVariance({7.0, 8.0, 9.0, 10.0}, mean);

    // 1) Test Compute()
    std::vector<double> outputVec;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }
    if (hasOutput)
    {
        outputVec = features.GetOutput();
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    std::cout << "variance: " << outputVec[0] << ", expected " << expectedOutput << std::endl;
    testing::ProcessTest("Testing Variance Feature", testing::IsEqual(outputVec[0], expectedOutput));

    // 2) Create Model / Map and test it
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);
    
    std::cout << "Variance feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it    
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    double output;
    if (outputVec.size() == 0)
    {
        testing::ProcessTest("Error: no output from VarianceFeature's Map", false); 
    }
    testing::ProcessTest("Testing VarianceFeature", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestIncrementalVarianceFeature()
{
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto varianceFeature = features.CreateFeature<IncrementalVarianceFeature>(inputFeature, windowSize);
    features.SetOutputFeature(varianceFeature);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double mean = VectorMean({7.0, 8.0, 9.0, 10.0});
    double expectedOutput = VectorVariance({7.0, 8.0, 9.0, 10.0}, mean);

    // 1) Test Compute()
    std::vector<double> outputVec;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }

    if (hasOutput)
    {
        outputVec = features.GetOutput();
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    testing::ProcessTest("Testing Variance Feature", testing::IsEqual(outputVec[0], expectedOutput));

    // 2) Create Model / Map and test it
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);
    
    std::cout << "Variance feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it    
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    if (outputVec.size() == 0)
    {
        testing::ProcessTest("Error: no output from VarianceFeature's Map", false); 
    }
    testing::ProcessTest("Testing VarianceFeature", testing::IsEqual(outputVec[0], expectedOutput));
}
