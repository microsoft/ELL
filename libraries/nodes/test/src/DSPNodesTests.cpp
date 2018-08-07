////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPNodesTest.cpp (nodes_test)
//  Authors:  Chuck Jacobs, Byron Changuion, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DSPNodesTests.h"
#include "DTWPrototype.h"
#include "NodesTestData.h"
#include "ModelTestUtilities.h"

// common
#include "LoadModel.h"

// dsp
#include "Convolution.h"

// math
#include "MathConstants.h"
#include "Tensor.h"
#include "TensorOperations.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "Node.h"

// nodes
#include "BufferNode.h"
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DiagonalConvolutionNode.h"
#include "FFTNode.h"
#include "FilterBankNode.h"
#include "GRULayerNode.h"
#include "IIRFilterNode.h"
#include "LSTMLayerNode.h"
#include "RecurrentLayerNode.h"
#include "SimpleConvolutionNode.h"
#include "UnrolledConvolutionNode.h"
#include "VoiceActivityDetectorNode.h"
#include "WinogradConvolutionNode.h"

// predictors
#include "NeuralNetworkPredictor.h"
#include "SigmoidActivation.h"
#include "TanhActivation.h"

// predictors/neural
#include "ConvolutionalLayer.h"

// testing
#include "testing.h"

// data
#include "Example.h"
#include "Dataset.h"
#include "DataLoaders.h"
#include "WeightLabel.h"

// utilities
#include "Exception.h"
#include "Files.h"
#include "RandomEngines.h"
#include "StringUtil.h"

// stl
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>

using namespace ell;
using namespace nodes;
using namespace data;
using namespace std::string_literals;

//
// Helpers
//
namespace
{

struct ImageShape
{
    int numRows;
    int numColumns;
    int numChannels;
};

struct FiltersShape
{
    int numFilters;
    int numRows;
    int numColumns;
    int numChannels;
};

// Parameter struct for passing options in to Winograd convolution tests.
struct WinogradOptions
{
    int tileSize;
    dsp::WinogradFilterOrder filterOrder;
};

struct SimpleOptions
{
};

struct UnrolledOptions
{
};

struct DiagonalOptions
{
};

union ConvolutionOptions
{
    ConvolutionOptions() {}
    ConvolutionOptions(int tileSize, dsp::WinogradFilterOrder order)
        : winogradOptions({ tileSize, order }) {}
    ConvolutionOptions(int tileSize)
        : winogradOptions({ tileSize, dsp::WinogradFilterOrder::tilesFirst }) {}

    WinogradOptions winogradOptions;
    SimpleOptions simpleOptions;
    UnrolledOptions unrolledOptions;
    DiagonalOptions diagonalOptions;
};

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
// Test compute functions
//

static void TestDelayNodeCompute()
{
    const int delay = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::DelayNode<double>>(inputNode->output, delay);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    std::vector<double> outputVec;

    for (size_t index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        inputNode->SetInput(inputValue);
        outputVec = model.ComputeOutput(outputNode->output);
        if (index >= delay)
        {
            testing::ProcessTest("Testing DelayNode compute", testing::IsEqual(outputVec, data[index - delay]));
        }
    }
}

static void TestFFTNodeCompute()
{
    using ValueType = double;
    const size_t N = 32;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(N);
    auto fftNode = model.AddNode<nodes::FFTNode<ValueType>>(inputNode->output);

    // FFT of constant value
    std::vector<ValueType> signal(N, 1.0);
    inputNode->SetInput(signal);
    auto computeOutput = model.ComputeOutput(fftNode->output);
    for (size_t index = 0; index < computeOutput.size(); ++index)
    {
        auto x = computeOutput[index];
        testing::ProcessTest("Testing real-valued FFT of DC signal", testing::IsEqual(x, static_cast<ValueType>(index == 0 ? N : 0)));
    }

    // FFT of impulse signal
    signal.assign(N, 0);
    signal[0] = 1.0;
    inputNode->SetInput(signal);
    computeOutput = model.ComputeOutput(fftNode->output);
    for (size_t index = 0; index < computeOutput.size(); ++index)
    {
        auto x = computeOutput[index];
        testing::ProcessTest("Testing real-valued FFT of impulse signal", testing::IsEqual(x, static_cast<ValueType>(1)));
    }

    // FFT of some arbitrary sine waves
    for (size_t freq : { 1, 3, 6, 11 })
    {
        for (size_t index = 0; index < N; ++index)
        {
            signal[index] = std::sin(2 * math::Constants<ValueType>::pi * index * freq / N);
        }
        inputNode->SetInput(signal);
        computeOutput = model.ComputeOutput(fftNode->output);
        for (size_t index = 0; index < computeOutput.size(); ++index)
        {
            auto x = computeOutput[index];
            bool isPeak = (index == freq) || (index == (N - freq));
            testing::ProcessTest("Testing real-valued FFT of sine wave", testing::IsEqual(x, static_cast<ValueType>(isPeak ? N / 2 : 0)));
        }
    }
}

static void TestDTWDistanceNodeCompute()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto prototype = GetNextSlidePrototype();
    auto dtwNode = model.AddNode<nodes::DTWDistanceNode<double>>(inputNode->output, prototype);

    //
    auto prototypeLength = prototype.size();
    size_t numSamples = 200;
    size_t increment = 3;
    for (size_t index = 0; index < numSamples; ++index)
    {
        auto sampleIndex = (index * increment) % prototypeLength;
        auto inputValue = prototype[sampleIndex];
        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(dtwNode->output);
    }
}

//
// Combined tests
//

template <typename ValueType>
static void TestIIRFilterNode1()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    std::vector<std::vector<ValueType>> data = { { 1 }, { 0 }, { 0 }, { 0 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(data[0].size());
    auto outputNode = model.AddNode<nodes::IIRFilterNode<ValueType>>(inputNode->output, std::vector<ValueType>{ static_cast<ValueType>(1.0) }, std::vector<ValueType>{ static_cast<ValueType>(-0.95) });

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    std::vector<std::vector<ValueType>> expectedOutput = { { static_cast<ValueType>(1.0) }, { static_cast<ValueType>(0.95) }, { static_cast<ValueType>(0.95 * 0.95) }, { static_cast<ValueType>(0.95 * 0.95 * 0.95) } };
    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

        testing::ProcessTest("Testing IIRFilterNode compute", testing::IsEqual(computedResult, expectedOutput[index], epsilon));
        testing::ProcessTest("Testing IIRFilterNode compile", testing::IsEqual(compiledResult, expectedOutput[index], epsilon));
    }
}

template <typename ValueType>
static void TestIIRFilterNode2()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    std::vector<std::vector<ValueType>> data = { { 1, 0, 0, 0 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(data[0].size());
    auto outputNode = model.AddNode<nodes::IIRFilterNode<ValueType>>(inputNode->output, std::vector<ValueType>{ static_cast<ValueType>(1.0) }, std::vector<ValueType>{ static_cast<ValueType>(-0.95) });

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    std::vector<std::vector<ValueType>> expectedOutput = { { static_cast<ValueType>(1.0), static_cast<ValueType>(0.95), static_cast<ValueType>(0.95 * 0.95), static_cast<ValueType>(0.95 * 0.95 * 0.95) } };
    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

        testing::ProcessTest("Testing IIRFilterNode compute 2", testing::IsEqual(computedResult, expectedOutput[index], epsilon));
        testing::ProcessTest("Testing IIRFilterNode compile 2", testing::IsEqual(compiledResult, expectedOutput[index], epsilon));
    }
}

template <typename ValueType>
static void TestIIRFilterNode3()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    std::vector<ValueType> datapoint(128);
    datapoint[0] = 1.0;
    std::vector<std::vector<ValueType>> data = { datapoint };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(data[0].size());
    std::vector<ValueType> aCoeffs = { static_cast<ValueType>(0.0125), static_cast<ValueType>(-0.0125) };
    std::vector<ValueType> bCoeffs = { static_cast<ValueType>(1.0), static_cast<ValueType>(0.25), static_cast<ValueType>(-0.125) };
    auto outputNode = model.AddNode<nodes::IIRFilterNode<ValueType>>(inputNode->output, bCoeffs, aCoeffs);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

        testing::ProcessTest("Testing IIRFilterNode compile 3", testing::IsEqual(compiledResult, computedResult, epsilon));
    }
}

template <typename ValueType>
static void TestIIRFilterNode4()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    std::vector<std::vector<ValueType>> data = { { 1, 0, 0, 0, 0, 0, 0 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(data[0].size());
    std::vector<ValueType> aCoeffs = { 0 };
    std::vector<ValueType> bCoeffs = { static_cast<ValueType>(1.0), static_cast<ValueType>(0.25), static_cast<ValueType>(-0.125) };
    auto outputNode = model.AddNode<nodes::IIRFilterNode<ValueType>>(inputNode->output, bCoeffs, aCoeffs);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    std::vector<std::vector<ValueType>> expectedOutput = { bCoeffs };
    expectedOutput.resize(data.size());
    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

        testing::ProcessTest("Testing IIRFilterNode compute 4", testing::IsEqual(computedResult, expectedOutput[index], epsilon));
        testing::ProcessTest("Testing IIRFilterNode compile 4", testing::IsEqual(compiledResult, expectedOutput[index], epsilon));
    }
}

template <typename ValueType>
static void TestMelFilterBankNode()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);
    const size_t numFilters = 13;
    const size_t windowSize = 512;
    const double sampleRate = 16000;

    std::vector<ValueType> signal(windowSize);
    FillRandomVector(signal);
    std::vector<std::vector<ValueType>> data = { signal };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(windowSize);
    auto filters = dsp::MelFilterBank(windowSize, sampleRate, numFilters);
    auto outputNode = model.AddNode<nodes::MelFilterBankNode<ValueType>>(inputNode->output, filters);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

        testing::ProcessTest("Testing MelFilterBankNode compile", testing::IsEqual(compiledResult, computedResult, epsilon));
    }
}

template <typename ValueType>
static void TestBufferNode()
{
    const ValueType epsilon = static_cast<ValueType>(1e-7);
    const size_t inputSize = 16;
    const size_t windowSize = 32;

    std::vector<std::vector<ValueType>> data;
    const int numEntries = 8;
    for (int index = 0; index < numEntries; ++index)
    {
        std::vector<ValueType> item(inputSize);
        std::iota(item.begin(), item.end(), inputSize * index);
        data.push_back(item);
    }

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(inputSize);
    auto outputNode = model.AddNode<nodes::BufferNode<ValueType>>(inputNode->output, windowSize);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = false;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    for (size_t index = 0; index < data.size(); ++index)
    {
        auto input = data[index];

        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<ValueType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);
        testing::ProcessTest("Testing BufferNode compile", testing::IsEqual(compiledResult, computedResult, epsilon));
    }
}

template <typename ValueType>
static void TestConvolutionNodeCompile(dsp::ConvolutionMethodOption convolutionMethod)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const ValueType epsilon = static_cast<ValueType>(1e-7);
    const int inputRows = 10;
    const int inputColumns = 10;
    const int outputRows = 10;
    const int outputColumns = 10;
    const int numChannels = 1;
    const int filterSize = 3;
    const int numFilters = 1;
    const int inputPadding = 1;
    const int outputPadding = 0;
    const int stride = 1;

    const int winogradTileSize = 2;
    const auto winogradFilterOrder = nodes::WinogradConvolutionNode<ValueType>::FilterOrder::tilesFirst;

    auto data = GetConvolutionTestData<ValueType>();
    auto filter = GetConvolutionTestFilter<ValueType>();
    auto reference = GetCorrelationTestResultSame<ValueType>();

    auto inputMemoryLayout = CalculateMemoryLayout(inputRows, inputColumns, numChannels, inputPadding);
    auto outputMemoryLayout = CalculateMemoryLayout(outputRows, outputColumns, numFilters, outputPadding);
    auto filterWeights = Tensor(numFilters * filterSize, filterSize, numChannels, filter);

    // auto inputSize = data.size();
    auto inputSize = inputMemoryLayout.GetMemorySize();
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(inputSize);

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

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = false;
    settings.compilerSettings.useBlas = true;
    settings.verifyJittedModule = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // Check for errors in module
    auto& module = compiledMap.GetModule();
    auto hasErrors = module.CheckForErrors(std::cerr);
    testing::ProcessTest("Testing compiled "s + GetConvAlgName(convolutionMethod) + " convolution node model for errors", !hasErrors);

    compiledMap.SetInputValue(0, paddedDataArray);
    auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);
    auto ok = testing::IsEqual(reference, compiledResult, epsilon);

#if 0
    // Helpful debugging output
    if (!ok)
    {
        std::cout << "Compiled result: " << compiledResult << std::endl;
        std::cout << "Reference result: " << reference << std::endl;
    }
#endif
    testing::ProcessTest("Testing compiled "s + GetConvAlgName(convolutionMethod) + " convolution node", ok);
}

template <typename ValueType>
static void TestConvolutionNodeCompileVsReference(ImageShape inputShape, FiltersShape filterShape, int stride, dsp::ConvolutionMethodOption convolutionMethod, ConvolutionOptions options = {})
{
    int inputRows = inputShape.numRows;
    int inputColumns = inputShape.numColumns;
    int numChannels = inputShape.numChannels;

    int numFilters = filterShape.numFilters;
    int filterSize = filterShape.numRows;
    assert(filterShape.numColumns == filterSize);
    int numFilterChannels = filterShape.numChannels == 0 ? numChannels : filterShape.numChannels; // "0" means "number of input channels"
    auto isDepthwiseSeparable = (numFilterChannels == 1) && (numChannels > 1);

    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const ValueType epsilon = static_cast<ValueType>(1e-4);
    const int outputRows = inputRows / stride;
    const int outputColumns = inputColumns / stride;
    const int inputPadding = (filterSize - 1) / 2;
    const int outputPadding = 0;

    auto dataSize = inputRows * inputColumns * numChannels;
    auto data = std::vector<ValueType>(dataSize);
    FillRandomVector(data);

    auto filterWeightsSize = numFilters * filterSize * filterSize * numChannels;
    auto filter = std::vector<ValueType>(filterWeightsSize);
    FillRandomVector(filter);

    auto inputMemoryLayout = CalculateMemoryLayout(inputRows, inputColumns, numChannels, inputPadding);
    auto outputMemoryLayout = CalculateMemoryLayout(outputRows, outputColumns, numFilters, outputPadding);
    auto filterWeights = Tensor(numFilters * filterSize, filterSize, numFilterChannels, filter);

    auto inputSize = inputMemoryLayout.GetMemorySize();

    // Create compiler for models
    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.useBlas = true;
    settings.verifyJittedModule = true;

    model::IRMapCompiler compiler(settings);

    // Create "test" model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(inputSize);
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
        outputNode = model.AddNode<nodes::WinogradConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride, options.winogradOptions.tileSize, options.winogradOptions.filterOrder);
        break;
    }

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", model::PortElementsBase(*(outputNode->GetOutputPort(0))) } });

    auto rawDataTensor = Tensor(inputRows, inputColumns, numChannels, data);
    auto paddedDataTensor = Tensor(inputRows + 2 * inputPadding, inputColumns + 2 * inputPadding, numChannels);
    paddedDataTensor.Fill(0);
    auto dataTensorReference = paddedDataTensor.GetSubTensor(inputPadding, inputPadding, 0, inputRows, inputColumns, numChannels);
    dataTensorReference.CopyFrom(rawDataTensor);
    auto paddedDataArray = paddedDataTensor.ToArray();
    auto compiledMap = compiler.Compile(map);

    // Get reference value from dsp library
    std::vector<ValueType> reference;
    if (isDepthwiseSeparable)
    {
        reference = dsp::Convolve2DDepthwiseSeparable(paddedDataTensor, filterWeights, numFilters, stride).ToArray();
    }
    else
    {
        reference = dsp::Convolve2D(paddedDataTensor, filterWeights, numFilters, stride).ToArray();
    }

    compiledMap.SetInputValue(0, paddedDataArray);
    auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

    auto ok = testing::IsEqual(reference, compiledResult, epsilon);
    testing::ProcessTest("Testing compiled "s + GetConvAlgName(convolutionMethod) + " convolution node vs dsp reference", ok);

    // Helpful debugging output
    if (!ok)
    {
        std::vector<ValueType> diff(reference.size());
        for (size_t index = 0; index < reference.size(); ++index)
        {
            diff[index] = reference[index] - compiledResult[index];
        }
        auto minmax = std::minmax_element(diff.begin(), diff.end());

        std::cout << "Error processing compiled "s + GetConvAlgName(convolutionMethod) + " convolution node vs dsp reference for image size " << inputRows << " x " << inputColumns << " x " << numChannels;
        std::cout << " and " << numFilters << " " << filterSize << " x " << filterSize << " filters, with stride " << stride << "\n";
        std::cout << "  Min diff: " << *minmax.first << " max diff: " << *minmax.second << "\n";
#if 0
        if (compiledResult.size() < 500)
        {
            std::cout << "Compiled result:\n"
                      << compiledResult << "\n\n";
            std::cout << "Reference result:\n"
                      << reference << "\n\n";
        }
#endif
    }
}

//
// Recurrent layer nodes (Recurrent, GRU, LSTM)
//

// clang-format off
const float wData[] = { 0.0381341f, 0.55826f, -0.467607f, 0.264272f, -0.733331f, 0.464226f, 0.496708f,
0.0581872f, -0.514144f, 0.702823f, -1.50401f, 0.373703f, 0.885559f, -0.27592f,
-0.116469f, 0.320376f, -0.534044f, 1.92602f, -0.567954f, -0.0167191f, -0.822891f };
// clang-format on

void TestRecurrentNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType biases = VectorType({ -0.0773237, 0.909263, -0.297635 });

    MatrixType weights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            weights(i, j) = wData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~ 1,1,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };
    RecurrentLayer<ElementType> recurrent(parameters, weights, biases, new TanhActivation<ElementType>());
    recurrent.Compute();
    TensorType output = recurrent.GetOutput();

    recurrent.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto computeNode = model.AddNode<nodes::RecurrentLayerNode<ElementType>>(inputNode->output, recurrent);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerOptions settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

// clang-format off
const float uData[] = { -0.306974f, -0.314942f, -0.307079f, -0.0778356f, -0.0929513f, 0.0426045f, -0.0200071f,
0.508866f, 0.525531f, 0.345996f, -0.633406f, -0.519455f, 0.617442f, -0.0790342f,
2.13148f, 2.61342f, -2.99549f, -6.15958f, 0.224837f, 0.0745432f, 0.154865f };
const float rData[] = { -0.438305f, -0.438798f, -0.509791f, 0.385411f, -0.210201f, -0.302488f, 0.0717234f,
0.259852f, 0.532692f, 0.675258f, 0.0314993f, -0.609884f, -0.419196f, 0.407534f,
0.221932f, 0.51503f, -0.278936f, 0.673416f, 0.307534f, -0.176314f, 0.440408f };
const float hData[] = { 0.0364258f, 0.557955f, -0.467648f, 0.265914f, 0.343273f, -0.0306102f, -0.265686f,
0.241587f, 0.283854f, 0.232303f, -0.397746f, -0.191887f, -0.0618932f, -0.551409f,
0.847701f, 0.234382f, -0.107097f, -0.38192f, 0.074817f, 0.555262f, 0.479104f };
// clang-format on

void TestGRUNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType updateBias = VectorType({ 0.0, 0.0, 3.95111 });
    VectorType resetBias = VectorType({ 0.0, 0.0, 0.0 });
    VectorType hiddenBias = VectorType({ -0.0686757, 0.0, 0.281977 });

    MatrixType updateWeights(3, 7);
    MatrixType resetWeights(3, 7);
    MatrixType hiddenWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            updateWeights(i, j) = uData[columnIndex];
            resetWeights(i, j) = rData[columnIndex];
            hiddenWeights(i, j) = hData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~1,0,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };
    GRUParameters<ElementType> gruParams{ updateWeights, resetWeights, hiddenWeights, updateBias, resetBias, hiddenBias };
    GRULayer<ElementType> gru(parameters, gruParams, new TanhActivation<ElementType>(), new SigmoidActivation<ElementType>());
    gru.Compute();
    auto output = gru.GetOutput();
    UNUSED(output);
    gru.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto resetTriggerNode = model.AddNode<nodes::ConstantNode<int>>(0);
    auto computeNode = model.AddNode<nodes::GRULayerNode<ElementType>>(inputNode->output, resetTriggerNode->output, gru);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerOptions settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

// clang-format off
const float iData[] = { 0.739646f, 0.8501f, -2.15136f, -2.44612f, 0.0639512f, -0.0492275f, 0.167204f,
-0.49359f, 0.253341f, -0.239276f, 0.114082f, -0.360225f, 0.434314f, -0.28489f,
-0.573704f, -0.0273829f, 0.0242156f, -0.600619f, -0.258574f, -0.312928f, -0.0446059f };
const float fData[] = { 0.0628231f, 0.145727f, -0.258802f, -0.57547f, -0.511279f, -0.470488f, 0.231888f,
0.42041f, -0.440816f, -0.343813f, 0.463799f, -0.456978f, 0.081054f, 0.532126f,
0.51855f, -0.123881f, 0.509249f, 0.324012f, 0.318677f, -0.411882f, 0.082f };
const float cData[] = { 0.187203f, 0.863434f, 0.490011f, -0.216801f, -0.290302f, 0.338456f, -0.216217f,
-0.000121037f, 0.0000392739f, 0.00000052499f, 0.0000676336f, 0.196989f, 0.312441f, 0.355654f,
0.468885f, -0.236218f, 0.415782f, 0.302927f, -0.0503453f, -0.183221f, -0.500112f };
const float oData[] = { 0.517059f, 0.470772f, -0.919974f, -0.319515f, 0.224966f, 0.195129f, 0.306053f,
0.261489f, 0.499691f, 0.132338f, 0.47862f, 0.21803f, 0.00246173f, -0.0274337f,
-0.385968f, 0.120127f, -0.360038f, -0.21129f, 0.0611264f, -0.17212f, -0.165724f };
// clang-format on
void TestLSTMNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType inputBias = VectorType({ 0.747351, -0.112848, 0.0 });
    VectorType forgetMeBias = VectorType({ 1.0, 1.0, 1.0 });
    VectorType candidateBias = VectorType({ 0.733668, 0.000431956, 0.0 });
    VectorType outputBias = VectorType({ 0.385433, 0.0, 0.0 });

    MatrixType inputWeights(3, 7);
    MatrixType forgetMeWeights(3, 7);
    MatrixType candidateWeights(3, 7);
    MatrixType outputWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            inputWeights(i, j) = iData[columnIndex];
            forgetMeWeights(i, j) = fData[columnIndex];
            candidateWeights(i, j) = cData[columnIndex];
            outputWeights(i, j) = oData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output 1,0,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    LSTMParameters<ElementType> lstmParams{ inputWeights, forgetMeWeights, candidateWeights, outputWeights, inputBias, forgetMeBias, candidateBias, outputBias };
    LSTMLayer<ElementType> lstm(parameters, lstmParams, new TanhActivation<ElementType>(), new SigmoidActivation<ElementType>());
    lstm.Compute();
    auto output = lstm.GetOutput();
    UNUSED(output);
    lstm.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto resetTriggerNode = model.AddNode<nodes::ConstantNode<int>>(0);
    auto computeNode = model.AddNode<nodes::LSTMLayerNode<ElementType>>(inputNode->output, resetTriggerNode->output, lstm);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerOptions settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    
    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

template< typename ElementType>
static Dataset<Example<DenseDataVector<ElementType>, WeightLabel>> LoadVadData(const std::string& path, int numFeatures)
{
    std::string filename = utilities::JoinPaths(path, { "..", "..", "dsp", "VadData.txt" });
    if (!utilities::FileExists(filename))
    {
        filename = utilities::JoinPaths(path, { "..", "dsp", "VadData.txt" });
    }
    // load the dataset
    auto stream2 = utilities::OpenIfstream(filename);
    Dataset<Example<DenseDataVector<ElementType>, WeightLabel>> dataset;
    AutoSupervisedExampleIterator exampleIterator = ell::common::GetAutoSupervisedExampleIterator(stream2);
    while (exampleIterator.IsValid())
    {
        auto example = exampleIterator.Get();
        auto vector = example.GetDataVector().ToArray();
        std::vector<ElementType> buffer;
        std::transform(vector.begin(), vector.end(), std::back_inserter(buffer), [](double x) { return static_cast<ElementType>(x); });
        dataset.AddExample(Example<DenseDataVector<ElementType>, WeightLabel>(DenseDataVector<ElementType>(buffer), example.GetMetadata()));
        exampleIterator.Next();
    }
    return dataset;
}

void TestWithSerialization(model::Map& map, std::string name, std::function<void(model::Map& map, int)> body)
{
    // 2 iterations is important, because it finds bugs in reserialization of the deserialized model.
    int iteration = 0;
    while (iteration++ < 3)
    {
        body(map, iteration);

        auto filename = name + ".json";

        // archive the model
        common::SaveMap(map, filename);

        // unarchive the model
        map = common::LoadMap(filename);
    }
}

const int FrameSize = 40;
const int SampleRate = 8000;
const double FrameDuration = 0.032; // shift of 256 and 256/8000=0.032.
const double TauUp = 1.54;
const double TauDown = 0.074326;
const double LargeInput = 2.400160;
const double GainAtt = 0.002885;
const double ThresholdUp = 3.552713;
const double ThresholdDown = 0.931252;
const double LevelThreshold = 0.007885;

static void TestVoiceActivityDetectorNode(const std::string& path)
{
    using ElementType = double;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<ElementType>>(FrameSize);
    auto outputNode = model.AddNode<nodes::VoiceActivityDetectorNode<ElementType>>(inputNode->output, SampleRate, FrameDuration, TauUp, TauDown, LargeInput, GainAtt, ThresholdUp, ThresholdDown, LevelThreshold);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });

    // Dump the module so we can debug it
    // compiledMap.GetModule().DebugDump();
    auto dataset = LoadVadData<ElementType>(path, FrameSize);

    TestWithSerialization(map, "TestVoiceActivityDetectorNode", [&dataset](model::Map& map, int iteration){

        // compiling it.
        model::MapCompilerOptions settings;
        settings.verifyJittedModule = true;
        settings.compilerSettings.optimize = false;
        settings.compilerSettings.debug = true;
        model::IRMapCompiler compiler(settings);
        auto compiledMap = compiler.Compile(map);

        // now test that it works.
        int refErrors = 0;
        int compileErrors = 0;
        size_t numFrames = dataset.NumExamples();
        for (size_t frame = 0; frame < numFrames; frame++)
        {
            auto e = dataset.GetExample(frame);
            std::vector<ElementType> buffer = e.GetDataVector().ToArray();
            if (buffer.size() < FrameSize)
            {
                buffer.resize(FrameSize);
            }
            int expectedSignal = static_cast<int>(e.GetMetadata().label);

            map.SetInputValue("input", buffer);
            std::vector<int> outputVec = map.ComputeOutput<int>("output");
            int signal = outputVec[0];
            if (signal != expectedSignal)
            {
                ++refErrors;
            }

            compiledMap.SetInputValue(0, buffer);
            auto outputVec2 = compiledMap.ComputeOutput<int>(0);
            signal = outputVec2[0];
            if (signal != expectedSignal)
            {
                ++compileErrors;
            }

        } 

        testing::ProcessTest(utilities::FormatString("Testing TestVoiceActivityDetectorNode Compute iteration %d, %d errors", iteration, refErrors), refErrors == 0);
        testing::ProcessTest(utilities::FormatString("Testing TestVoiceActivityDetectorNode Compiled iteration %d, %d errors", iteration, compileErrors), compileErrors == 0);

    });
}

void TestGRUNodeWithVADReset(const std::string& path)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    auto dataset = LoadVadData<ElementType>(path, FrameSize);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(FrameSize);
    auto vadNode = model.AddNode<nodes::VoiceActivityDetectorNode<ElementType>>(inputNode->output, SampleRate, FrameDuration, TauUp, TauDown, LargeInput, GainAtt, ThresholdUp, ThresholdDown, LevelThreshold);

    size_t inputSize = FrameSize;
    size_t hiddenUnits = 10;

    VectorType updateBias(std::vector<ElementType>(hiddenUnits, static_cast<ElementType>(0.01)));
    VectorType resetBias(std::vector<ElementType>(hiddenUnits, static_cast<ElementType>(0.02)));
    VectorType hiddenBias(std::vector<ElementType>(hiddenUnits, static_cast<ElementType>(0.01)));

    size_t matrixSize = hiddenUnits * (hiddenUnits + inputSize);
    MatrixType updateWeights(hiddenUnits, hiddenUnits + inputSize, std::vector<ElementType>(matrixSize, static_cast<ElementType>(0.01)));
    MatrixType resetWeights(hiddenUnits, hiddenUnits + inputSize, std::vector<ElementType>(matrixSize, static_cast<ElementType>(0.01)));
    MatrixType hiddenWeights(hiddenUnits, hiddenUnits + inputSize, std::vector<ElementType>(matrixSize, static_cast<ElementType>(0.01)));

    TensorType input(1, 1, FrameSize);
    Shape outputShape = { 1, 1, hiddenUnits };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    GRUParameters<ElementType> gruParams{ updateWeights, resetWeights, hiddenWeights, updateBias, resetBias, hiddenBias };
    GRULayer<ElementType> gru(parameters, gruParams, new TanhActivation<ElementType>(), new SigmoidActivation<ElementType>());

    auto gruNode = model.AddNode<nodes::GRULayerNode<ElementType>>(inputNode->output, vadNode->output, gru);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", gruNode->output } });

    int iteration = 0;
    while (iteration++ < 3)
    {
        // now test compiling it.
        model::MapCompilerOptions settings;
        settings.verifyJittedModule = true;
        settings.compilerSettings.optimize = false;
        settings.compilerSettings.debug = true;
        model::IRMapCompiler compiler(settings);
        auto compiledMap = compiler.Compile(map);
        int errors = 0;

        size_t numFrames = dataset.NumExamples();
        int lastSignal = 0;
        bool wasReset = false;
        for (size_t frame = 0; frame < numFrames; frame++)
        {
            auto e = dataset.GetExample(frame);
            std::vector<ElementType> buffer = e.GetDataVector().ToArray();
            if (buffer.size() < FrameSize)
            {
                buffer.resize(FrameSize);
            }
            int expectedSignal = static_cast<int>(e.GetMetadata().label);

            compiledMap.SetInputValue(0, buffer);
            auto outputVec = compiledMap.ComputeOutput<ElementType>(0);
            ElementType sum = 0;
            for (auto x : outputVec)
            {
                sum += x;
            }
            if (wasReset && sum > static_cast<ElementType>(0.1))
            {
                errors++;
            }
            if (lastSignal == 1 && expectedSignal == 0)
            {
                // reset should have happened which means the next sum must be close to zero.
                wasReset = true;
            }
            else
            {
                wasReset = false;
            }

            lastSignal = expectedSignal;
        }
        hiddenUnits = 0;

        testing::ProcessTest(utilities::FormatString("Testing TestGRUNodeWithVADReset iteration %d, %d errors", iteration, errors), errors == 0);

        // archive the model
        common::SaveMap(map, "TestGRUNodeWithVADReset.json");

        // unarchive the model
        map = common::LoadMap("TestGRUNodeWithVADReset.json");
    }
}

//
// Main driver function to call all the tests
//
void TestDSPNodes(const std::string& path)
{
    TestVoiceActivityDetectorNode(path);
    TestGRUNodeWithVADReset(path);

    //
    // Compute tests
    //
    TestDelayNodeCompute();
    TestDTWDistanceNodeCompute();
    TestFFTNodeCompute();

    //
    // Combined tests
    //
    TestIIRFilterNode1<float>();
    TestIIRFilterNode2<float>();
    TestIIRFilterNode3<float>();
    TestIIRFilterNode4<float>();

    TestMelFilterBankNode<float>();
    TestMelFilterBankNode<double>();

    TestBufferNode<float>();

    TestRecurrentNode();
    TestGRUNode();
    TestLSTMNode();

    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::simple);
    // TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::diagonal); // ERROR: diagonal test currently broken
    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::winograd);

    // Test simple convolution
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 2, dsp::ConvolutionMethodOption::simple);

    // Test unrolled convolution
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 2, dsp::ConvolutionMethodOption::unrolled);

    // Test Winograd convolution with tile size 2
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::tilesFirst });
    // TestConvolutionNodeCompileVsReference<float>({120, 80, 8}, {16, 3, 3, 0}, 2, dsp::ConvolutionMethodOption::winograd, {2, dsp::WinogradFilterOrder::tilesFirst}); // Commented-out because Winograd doesn't support non-1 stride

    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });

    // Test Winograd convolution with tile size 4
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::tilesFirst });
    // TestConvolutionNodeCompileVsReference<float>({120, 80, 8}, {16, 3, 3, 0}, 2, dsp::ConvolutionMethodOption::winograd, {4, dsp::WinogradFilterOrder::tilesFirst}); // Commented-out because Winograd doesn't support non-1 stride

    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 2, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 7, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 16, 3, 3, 0 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });

    //
    // Depthwise-separable convolution tests
    //
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 4, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);

    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 4, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::simple);

    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 4, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 2, dsp::WinogradFilterOrder::filtersFirst });
    
    
    TestConvolutionNodeCompileVsReference<float>({ 2, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 2, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 2, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 3, 3, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 4, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 4, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 4, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 2 }, { 2, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 5, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 5, 15, 4 }, { 4, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 8, 8, 1 }, { 1, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 32, 32, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 64, 64, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
    TestConvolutionNodeCompileVsReference<float>({ 120, 80, 8 }, { 8, 3, 3, 1 }, 1, dsp::ConvolutionMethodOption::winograd, { 4, dsp::WinogradFilterOrder::filtersFirst });
}
