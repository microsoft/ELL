////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledExerciseModel_main.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// compiled model
#include "compiled_model_noprofile.h"

// stl
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

struct ProfileArguments
{
    int numIterations;
    int numWarmUpIterations;
};

//
// Test-data-related
//
template<typename T>
std::vector<T> GetInputData(std::string filename, const TensorShape& inputShape, float scale)
{
    std::default_random_engine engine(123);
    auto inputSize = inputShape.rows * inputShape.columns * inputShape.channels;
    std::vector<T> result(inputSize);
    std::uniform_real_distribution<double> dist;
    for (auto index = 0; index < inputSize; ++index)
    {
        result[index] = static_cast<T>(dist(engine));
    }
    return result;
}

template<typename InputType, typename OutputType>
void RunModel(const ProfileArguments& profileArguments)
{
    TensorShape inputShape;
    TensorShape outputShape;
    ELL_GetInputShape(0, &inputShape);
    ELL_GetOutputShape(0, &outputShape);

    auto inputSize = ELL_GetInputSize();
    auto outputSize = ELL_GetOutputSize();

    std::vector<InputType> input(inputSize);
    std::vector<OutputType> output(outputSize);

    // Evaluate the model in a loop
    for (int iter = 0; iter < profileArguments.numIterations; ++iter)
    {
        // Exercise the model
        ELL_Predict(input.data(), output.data());
    }
}

int main(int argc, char* argv[])
{
    using InputType = float;
    using OutputType = float;

    int numIterations = 20;
    if(argc > 1)
    {
        numIterations = atoi(argv[1]);
    }

    std::cout << "Performing " << numIterations << " iterations of predict function" << std::endl;
    // add arguments to the command line parser
    ProfileArguments profileArguments;
    profileArguments.numIterations = numIterations;
    RunModel<InputType, OutputType>(profileArguments);

    return 0;
}
