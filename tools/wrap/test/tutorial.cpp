////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     tutorial.cpp
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdio.h>
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

class TutorialWrapper : public ModelWrapper
{
public:
    void SourceCallback(std::vector<double>& input0) override
    {
        size_t size = GetInputSize(0);
        input0.resize(size);
        for (size_t i = 0; i < size; i++)
        {
            input0[i] = 1.0;
        }
    }
};

int main(int argc, char** argv)
{
    // Create a vector to hold the input to the model
    TutorialWrapper wrapper;

    // Send the image to the compiled model and fill the predictions vector with scores and measure how long it takes
    auto start = std::chrono::steady_clock::now();
    auto predictions = wrapper.Predict();
    auto end = std::chrono::steady_clock::now();

    std::cout << "Prediction=" << str(predictions.begin(), predictions.end()) << std::endl;

    auto duration = std::chrono::duration<double>(end - start).count();
    std::cout << "Prediction time: " << duration << " s" << std::endl;

    //if result != "1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0":
    bool failed = false;
    for (size_t i = 0, n = predictions.size(); i < n; i++)
    {
        if (predictions[i] != i + 1)
        {
            failed = true;
            break;
        }
    }
    if (failed)
    {
        std::cout << "### FAILED tutorial.cpp got unexpected output from Predict function\n";
        return 1;
    }

    return 0;
}