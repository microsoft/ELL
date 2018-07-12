////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPNodesTiming.cpp (nodes_test)
//  Authors:  Chuck Jacobs, Byron Changuion, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DSPNodesTests.h"

// dsp
#include "Convolution.h"
#include "WinogradConvolution.h"

// math
#include "MathConstants.h"
#include "Tensor.h"
#include "TensorOperations.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "Node.h"

// nodes
#include "DiagonalConvolutionNode.h"
#include "SimpleConvolutionNode.h"
#include "UnrolledConvolutionNode.h"
#include "WinogradConvolutionNode.h"

// predictors/neural
#include "ConvolutionalLayer.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"
#include "MillisecondTimer.h"
#include "RandomEngines.h"

// stl
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

using namespace ell;
using namespace nodes;
using namespace std::string_literals;

//
// Helpers
//
namespace
{
template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vec)
{
    os << "[";
    for (auto x : vec)
    {
        os << x << " ";
    }
    os << "]";
    return os;
}

template <typename ElementType>
void FillRandomVector(std::vector<ElementType>& vector, ElementType min = -1, ElementType max = 1)
{
    auto randomEngine = utilities::GetRandomEngine("123");
    std::uniform_real_distribution<ElementType> uniform(min, max);
    auto rand = [&randomEngine, &uniform]() { return uniform(randomEngine); };
    std::generate(vector.begin(), vector.end(), rand);
}

template <typename ElementType>
void FillDataVector(std::vector<ElementType>& vector, int numRows, int numColumns, int numChannels)
{
    int vectorIndex = 0;
    for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (int columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                ElementType rowValue = rowIndex < 3 && columnIndex < 2 ? 2 * rowIndex : 0;
                ElementType columnValue = rowIndex < 3 && columnIndex < 2 ? columnIndex + 1 : 0;
                vector[vectorIndex++] = rowValue + columnValue;
            }
        }
    }
}

std::string GetConvAlgName(dsp::ConvolutionMethodOption alg)
{
    switch (alg)
    {
    case dsp::ConvolutionMethodOption::automatic:
        return "automatic";
    case dsp::ConvolutionMethodOption::simple:
        return "simple";
    case dsp::ConvolutionMethodOption::unrolled:
        return "unrolled";
    case dsp::ConvolutionMethodOption::diagonal:
        return "diagonal";
    case dsp::ConvolutionMethodOption::winograd:
        return "winograd";
    }
    return "";
}

model::PortMemoryLayout CalculateMemoryLayout(int numRows, int numColumns, int numChannels, int padding)
{
    // Calculate dimension parameters
    model::MemoryShape size{ numRows, numColumns, numChannels };
    model::MemoryShape offset{ padding, padding, 0 };
    model::MemoryShape stride{ numRows + 2 * padding, numColumns + 2 * padding, numChannels };

    return { size, stride, offset };
}
}

//
// Timing functions
//

template <typename ValueType>
auto TimeReferenceConvolution(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, int numIterations, dsp::ConvolutionMethodOption algorithm)
{
    const auto filterSize = static_cast<int>(filters.NumColumns());

    // Perform the convolution
    utilities::MillisecondTimer timer;
    if(algorithm == dsp::ConvolutionMethodOption::winograd)
    {
        const auto order = dsp::WinogradFilterOrder::tilesFirst;
        const int tileSize = 2;
        auto transformedFilters = dsp::GetTransformedFilters(filters, numFilters, tileSize, order);
        timer.Reset();
        for (int iter = 0; iter < numIterations; ++iter)
        {
            volatile auto result = Convolve2DWinogradPretransformed(signal, transformedFilters, numFilters, tileSize, filterSize, order);
        }
    }
    else
    {
        for (int iter = 0; iter < numIterations; ++iter)
        {
            volatile auto result = Convolve2D(signal, filters, static_cast<int>(numFilters), algorithm);
        }
    }
    auto duration = timer.Elapsed();
    return duration;
}

template <typename ValueType>
static void TimeConvolutionNode(int inputRows, int inputColumns, int numChannels, int numFilters, int numIterations, dsp::ConvolutionMethodOption convolutionMethod)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const int outputRows = inputRows;
    const int outputColumns = inputColumns;
    const int filterSize = 3;
    const int inputPadding = 1;
    const int outputPadding = 0;
    const int stride = 1;
    
    const int winogradTileSize = 2;
    const auto winogradFilterOrder = nodes::WinogradConvolutionNode<ValueType>::FilterOrder::tilesFirst;
    
    auto inputSize = (inputRows + 2 * inputPadding) * (inputColumns + 2 * inputPadding) * numChannels;
    auto totalFilterSize = filterSize * filterSize * numFilters * numChannels;

    auto data = std::vector<ValueType>(inputSize);
    auto filter = std::vector<ValueType>(totalFilterSize);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(inputSize);

    auto inputMemoryLayout = CalculateMemoryLayout(inputRows, inputColumns, numChannels, inputPadding);
    auto outputMemoryLayout = CalculateMemoryLayout(outputRows, outputColumns, numFilters, outputPadding);
    auto filterWeights = Tensor(numFilters * filterSize, filterSize, numChannels, filter);

    model::Node* outputNode = nullptr;
    switch (convolutionMethod)
    {
    case dsp::ConvolutionMethodOption::automatic:
        std::cout << "Testing 'automatic' method --- using 'simple' instead" << std::endl;
    // fallthrough
    case dsp::ConvolutionMethodOption::simple:
        outputNode = model.AddNode<nodes::SimpleConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride);
        break;
    case dsp::ConvolutionMethodOption::diagonal:
        outputNode = model.AddNode<nodes::DiagonalConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride);
        break;
    case dsp::ConvolutionMethodOption::unrolled:
        outputNode = model.AddNode<nodes::UnrolledConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride);
        break;
    case dsp::ConvolutionMethodOption::winograd:
        outputNode = model.AddNode<nodes::WinogradConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride, winogradTileSize, winogradFilterOrder);
        break;
    }

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", model::PortElementsBase(*(outputNode->GetOutputPort(0))) } });

    auto rawDataTensor = Tensor(inputRows, inputColumns, numChannels, data);
    auto paddedDataTensor = Tensor(inputRows + 2, inputColumns + 2, numChannels);
    paddedDataTensor.Fill(0);

    auto dataTensorReference = paddedDataTensor.GetSubTensor(inputPadding, inputPadding, 0, inputRows, inputColumns, numChannels);
    dataTensorReference.CopyFrom(rawDataTensor);
    auto paddedDataArray = paddedDataTensor.ToArray();

    utilities::MillisecondTimer timer;

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.useBlas = true;
    settings.compilerSettings.parallelize = false;
    settings.verifyJittedModule = true;
    model::IRMapCompiler compiler(settings);

    timer.Reset();
    auto compiledMap = compiler.Compile(map);
    auto compilationTime = timer.Elapsed();
    std::cout << "Time to compile model: " << compilationTime << " ms\n";


    timer.Reset();
    for (int index = 0; index < numIterations; ++index)
    {
        compiledMap.SetInputValue(0, paddedDataArray);
        volatile auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);
    }
    auto compiledTime = timer.Elapsed();

    // Compare against reference version from dsp library
    auto referenceTime = TimeReferenceConvolution(paddedDataTensor, filterWeights, numFilters, numIterations, convolutionMethod);

    auto algName = GetConvAlgName(convolutionMethod);
    std::cout << "Total time for " << numIterations << " iterations of " << inputRows << " x " << inputColumns << " x " << numChannels << " -> " << numFilters << " " << algName << " convolutions: " << compiledTime << " ms\t" << "(reference: " << referenceTime << " ms)\n";
}

//
// Main driver function to call all the timing functions
//
void TimeDSPNodes()
{
    //
    // Timings on jitted models 
    //
    TimeConvolutionNode<float>(240, 240, 3, 16, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(240, 240, 3, 16, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(240, 240, 3, 16, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;

    TimeConvolutionNode<float>(100, 100, 16, 32, 10, dsp::ConvolutionMethodOption::simple); 
    TimeConvolutionNode<float>(100, 100, 16, 32, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(100, 100, 16, 32, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;

    TimeConvolutionNode<float>(32, 48, 64, 256, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(32, 48, 64, 256, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(32, 48, 64, 256, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;

    TimeConvolutionNode<float>(64, 64, 16, 16, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(64, 64, 16, 16, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(64, 64, 16, 16, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;
    
    TimeConvolutionNode<float>(64, 64, 32, 32, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(64, 64, 32, 32, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(64, 64, 32, 32, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;
    
    TimeConvolutionNode<float>(64, 64, 64, 64, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(64, 64, 64, 64, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(64, 64, 64, 64, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;
    
    TimeConvolutionNode<float>(64, 64, 128, 128, 10, dsp::ConvolutionMethodOption::simple);
    TimeConvolutionNode<float>(64, 64, 128, 128, 10, dsp::ConvolutionMethodOption::unrolled);
    TimeConvolutionNode<float>(64, 64, 128, 128, 10, dsp::ConvolutionMethodOption::winograd);
    std::cout << std::endl;


    // Winograd-specific stuff
    TimeConvolutionNode<float>(127, 127, 8, 8, 10, dsp::ConvolutionMethodOption::winograd);
    TimeConvolutionNode<float>(127, 127, 16, 16, 10, dsp::ConvolutionMethodOption::winograd);
    TimeConvolutionNode<float>(127, 127, 32, 32, 10, dsp::ConvolutionMethodOption::winograd);
    TimeConvolutionNode<float>(127, 127, 64, 64, 10, dsp::ConvolutionMethodOption::winograd);
    TimeConvolutionNode<float>(127, 127, 128, 128, 10, dsp::ConvolutionMethodOption::winograd);
    TimeConvolutionNode<float>(127, 127, 256, 256, 10, dsp::ConvolutionMethodOption::winograd);
}
