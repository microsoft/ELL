////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateTestModels_main.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateTestModels.h"

#include <common/include/LoadModel.h>

#include <model/include/Model.h>

#include <utilities/include/Exception.h>

#include <iostream>
#include <stdexcept>

using namespace ell;

void SaveModels()
{
    common::SaveMap(GenerateBinaryConvolutionModel(160, 160, 3, 8), "binary_conv_160x160x3x8.ell");
    common::SaveMap(GenerateBinaryConvolutionPlusDenseModel(160, 160, 3, 8, 10), "binary_conv_160x160x3x8-dense_10.ell");
    common::SaveMap(GenerateBinaryDarknetLikeModel(), "binary_darknet_160x160x3.ell");
    common::SaveMap(GenerateBinaryDarknetLikeModel(true), "binary_darknet_160x160x3_real.ell");

    common::SaveMap(GenerateConvolutionModel(64, 64, 4, 4, 3, 1, dsp::ConvolutionMethodOption::simple), "simple_64x64x4x8.ell");
    common::SaveMap(GenerateConvolutionModel(64, 64, 4, 4, 3, 1, dsp::ConvolutionMethodOption::unrolled), "unrolled_64x64x4x8.ell");
    common::SaveMap(GenerateConvolutionModel(64, 64, 4, 4, 3, 1, dsp::ConvolutionMethodOption::winograd), "winograd_64x64x4x8.ell");

    common::SaveMap(GenerateConvolutionModel(128, 128, 64, 64, 3, 1, dsp::ConvolutionMethodOption::simple), "simple_128x128x64x64.ell");
    common::SaveMap(GenerateConvolutionModel(128, 128, 64, 64, 3, 1, dsp::ConvolutionMethodOption::unrolled), "unrolled_128x128x64x64.ell");
    common::SaveMap(GenerateConvolutionModel(128, 128, 64, 64, 3, 1, dsp::ConvolutionMethodOption::winograd), "winograd_128x128x64x64.ell");
}

int main(int argc, char* argv[])
{
    try
    {
        SaveModels();
    }
    catch (utilities::Exception& e)
    {
        std::cout << "Exception: " << e.GetMessage() << std::endl;
        throw;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        throw;
    }
    return 0;
}