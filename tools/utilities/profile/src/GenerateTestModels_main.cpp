////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateTestModels_main.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateTestModels.h"

// common
#include "LoadModel.h"

// model
#include "Model.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>
#include <stdexcept>

using namespace ell;

void SaveModels()
{
    common::SaveMap(GenerateBinaryConvolutionModel(160, 160, 3, 8), "binary_conv_160x160x3x8.ell");
    common::SaveMap(GenerateBinaryConvolutionPlusDenseModel(160, 160, 3, 8, 10), "binary_conv_160x160x3x8-dense_10.ell");
    common::SaveMap(GenerateBinaryDarknetLikeModel(), "binary_darknet_160x160x3.ell");
    common::SaveMap(GenerateBinaryDarknetLikeModel(true), "binary_darknet_160x160x3_real.ell");
}

int main(int argc, char* argv[])
{
    try
    {
        SaveModels();
    }
    catch(utilities::Exception& e)
    {
        std::cout << "Exception: " << e.GetMessage() << std::endl;
        throw;
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        throw;
    }
    return 0;
}