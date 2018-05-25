////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     tutorial.cpp
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>

// Include the model interface file for the compiled ELL model
// Note: the "<modelname>_MAIN" preprocessor symbol must be defined in exactly one source file
//       that includes the model interface file. This is typically the file that defines "main()"
#define model_MAIN
#include "model.h"

template <typename T>
std::string str(T begin, T end)
{
    std::stringstream ss;
    bool first = true;
    for (; begin != end; begin++)
    {
        if (!first)
            ss << ", ";
        ss << *begin;
        first = false;
    }
    return ss.str();
}

int main(int argc, char** argv)
{
    model_PredictWrapper wrapper;

    // Create a vector to hold the input to the model
    std::vector<double> input(wrapper.GetInputSize());

    // Create a vector to hold the model's output predictions
    std::vector<double> predictions(wrapper.GetOutputSize());

    // Send the image to the compiled model and fill the predictions vector with scores and measure how long it takes
    auto start = std::chrono::steady_clock::now();
    wrapper.Predict(input, predictions);
    auto end = std::chrono::steady_clock::now();

    std::cout << "Prediction=" << str(predictions.begin(), predictions.end()) << std::endl;

    auto duration = std::chrono::duration<double>(end - start).count();
    std::cout << "Prediction time: " << duration << " s" << std::endl;

    return 0;
}