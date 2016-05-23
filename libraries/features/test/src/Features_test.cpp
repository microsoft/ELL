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
#include "VarianceFeature.h"

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
void TestMagnitudeFeatureCompute()
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
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    double expectedOutput = VectorMagnitude(data);
    testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(output, expectedOutput));
}

void TestMagnitudeFeatureModel()
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
    std::cout << "Magnitude feature output coordinates: " << outputCoordinates << std::endl;

    std::cout << "Magnitude feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    std::vector<double> data = { 1,2,3 };
    dataset::DoubleDataVector dataVector(data);
    auto outputVec = map.Compute(data);
    
    double output;
    if (outputVec.size() > 0)
    {
        output = outputVec[0];
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }
    double expectedOutput = VectorMagnitude(data);
    testing::ProcessTest("Testing MagnitudeFeature", testing::IsEqual(output, expectedOutput));        
}

void TestMeanFeatureCompute()
{
   FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto meanFeature = features.CreateFeature<MeanFeature>(inputFeature, windowSize);
    features.SetOutputFeature(meanFeature);
    features.Reset();

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double output = 0;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }
    if (hasOutput)
    {
        auto out = features.GetOutput();
        output = out[0];
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 
    testing::ProcessTest("Testing MeanFeature", testing::IsEqual(output, expectedOutput));
}

void TestMeanFeatureModel()
{
    // Set up feature set
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto meanFeature = features.CreateFeature<MeanFeature>(inputFeature, windowSize);
    features.SetOutputFeature(meanFeature);

    // create model
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);
    std::cout << "Mean feature output coordinates: " << outputCoordinates << std::endl;

    std::cout << "Mean feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };

    std::vector<double> outputVec;
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    double output;
    if (outputVec.size() > 0)
    {
        output = outputVec[0];
    }
    else
    {
        testing::ProcessTest("Error: no output from MeanFeature", false); 
    }
    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 
    testing::ProcessTest("Testing MeanFeature", testing::IsEqual(output, expectedOutput));
}

void TestVarianceFeatureCompute()
{
   FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto varianceFeature = features.CreateFeature<VarianceFeature>(inputFeature, windowSize);
    features.SetOutputFeature(varianceFeature);
    features.Reset();

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double output = 0;
    bool hasOutput = false;
    for(const auto& x: data)
    {
        hasOutput = features.ProcessInputData(x);
    }
    if (hasOutput)
    {
        auto out = features.GetOutput();
        output = out[0];
    }
    else
    {
        testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    }

    double mean = VectorMean({7.0, 8.0, 9.0, 10.0});
    double expectedOutput = VectorVariance({7.0, 8.0, 9.0, 10.0}, mean);
    std::cout << "variance: " << output << ", expected " << expectedOutput << std::endl;
    testing::ProcessTest("Testing Variance Feature", testing::IsEqual(output, expectedOutput));
}

void TestVarianceFeatureModel()
{
    // Set up feature set
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(1);
    const int windowSize = 4;
    auto varianceFeature = features.CreateFeature<VarianceFeature>(inputFeature, windowSize);
    features.SetOutputFeature(varianceFeature);

    // create model
    layers::Model model;
    layers::CoordinateList inputCoordinates(0, 1); // What should the input coordinates be???
    auto outputCoordinates = features.AddToModel(model, inputCoordinates);
    
    std::cout << "Variance feature model" << std::endl;
    model.Save(std::cout);
    std::cout << std::endl;
    
    // create a map
    layers::Map map(model, outputCoordinates);

    // now run a test vector through it
    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };

    std::vector<double> outputVec;
    for(const auto& x: data)
    {
        outputVec = map.Compute(x);
    }
    
    double output;
    if (outputVec.size() > 0)
    {
        output = outputVec[0];
    }
    else
    {
        testing::ProcessTest("Error: no output from VarianceFeature", false); 
    }
    double mean = VectorMean({7.0, 8.0, 9.0, 10.0}); 
    double expectedOutput = VectorVariance({7.0, 8.0, 9.0, 10.0}, mean);
    std::cout << "Output: " << output << ", expected: " << expectedOutput << std::endl;
    testing::ProcessTest("Testing VarianceFeature", testing::IsEqual(output, expectedOutput));
}
