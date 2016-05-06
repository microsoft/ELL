////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (features_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FeatureSet.h"
#include "Feature.h"
#include "InputFeature.h"
#include "UnaryFunctionFeature.h"

// testing
#include "testing.h"

// linear
#include "DoubleVector.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <iostream>
#include <string>
#include <sstream>

using namespace features;

void testGraph()
{
    FeatureSet features;
    auto inputFeature = features.CreateFeature<InputFeature>(3);
    auto magnitudeFeature = features.CreateFeature<MagnitudeFeature>(inputFeature);
    features.SetOutputFeature(magnitudeFeature);
    features.Reset();

    auto visitor = [](const Feature& v) {
        std::cout << v.Id() << std::endl;
    };
    features.Visit(visitor);

    std::vector<double> data = { 1,2,3 };
    double output = 0;
    bool hasOutput = features.ProcessInputData(data);
    if (hasOutput)
    {
        auto out = features.GetOutput();
        output = out[0];
        for (const auto& x : static_cast<std::vector<double>>(out))
        {
            std::cout << x << ", " << std::endl;
        }
    }
    else
    {
        std::cout << "No output" << std::endl;
    }

    double expectedOutput = std::sqrt((1 * 1) + (2 * 2) + (3 * 3));
    testing::ProcessTest("Testing simple feature set", testing::IsEqual(output, expectedOutput));
}

/// Runs all tests
///
int main()
{
    testGraph();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}




