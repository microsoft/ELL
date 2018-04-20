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
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DiagonalConvolutionNode.h"
#include "FFTNode.h"
#include "FilterBankNode.h"
#include "IIRFilterNode.h"
#include "SimpleConvolutionNode.h"
#include "UnrolledConvolutionNode.h"
#include "WinogradConvolutionNode.h"

// predictors
#include "NeuralNetworkPredictor.h"

// predictors/neural
#include "ConvolutionalLayer.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"
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
    model::Shape size{ numRows, numColumns, numChannels };
    model::Shape offset{ padding, padding, 0 };
    model::Shape stride{ numRows + 2 * padding, numColumns + 2 * padding, numChannels };

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

    auto data = GetConvolutionTestData<ValueType>();
    auto filter = GetConvolutionTestFilter<ValueType>();
    auto reference = GetCorrelationTestResultSame<ValueType>();

    auto inputMemoryLayout = CalculateMemoryLayout(inputRows, inputColumns, numChannels, inputPadding);
    auto outputMemoryLayout = CalculateMemoryLayout(outputRows, outputColumns, numFilters, outputPadding);
    auto filterWeights = Tensor(filter.data(), numFilters * filterSize, filterSize, numChannels);

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
        outputNode = model.AddNode<nodes::WinogradConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride);
        break;
    }

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", model::PortElementsBase(*(outputNode->GetOutputPort(0))) } });

    auto rawDataTensor = Tensor(data.data(), inputRows, inputColumns, numChannels);
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
static void TestConvolutionNodeCompileVsReference(int inputRows, int inputColumns, int numChannels, int numFilters, int filterSize, int stride, dsp::ConvolutionMethodOption convolutionMethod)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const ValueType epsilon = static_cast<ValueType>(1e-5);
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
    auto filterWeights = Tensor(filter.data(), numFilters * filterSize, filterSize, numChannels);

    auto inputSize = inputMemoryLayout.GetMemorySize();

    // Create compiler for models
    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.useBlas = true;
    settings.verifyJittedModule = true;

    model::IRMapCompiler referenceCompiler(settings);
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
        outputNode = model.AddNode<nodes::WinogradConvolutionNode<ValueType>>(inputNode->output, inputMemoryLayout, outputMemoryLayout, filterWeights, stride);
        break;
    }

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", model::PortElementsBase(*(outputNode->GetOutputPort(0))) } });

    auto rawDataTensor = Tensor(data.data(), inputRows, inputColumns, numChannels);
    auto paddedDataTensor = Tensor(inputRows + 2 * inputPadding, inputColumns + 2 * inputPadding, numChannels);
    paddedDataTensor.Fill(0);
    auto dataTensorReference = paddedDataTensor.GetSubTensor(inputPadding, inputPadding, 0, inputRows, inputColumns, numChannels);
    dataTensorReference.CopyFrom(rawDataTensor);
    auto paddedDataArray = paddedDataTensor.ToArray();
    auto compiledMap = compiler.Compile(map);

    // Get reference value from dsp library
    auto reference = dsp::Convolve2D(paddedDataTensor, filterWeights, numFilters, stride).ToArray();

    compiledMap.SetInputValue(0, paddedDataArray);
    auto compiledResult = compiledMap.ComputeOutput<ValueType>(0);

    auto ok = testing::IsEqual(reference, compiledResult, epsilon);
    testing::ProcessTest("Testing compiled "s + GetConvAlgName(convolutionMethod) + " convolution node vs dsp reference", ok);

    // Helpful debugging output
    if (!ok)
    {
        std::vector<ValueType> diff(reference.size());
        for(size_t index = 0; index < reference.size(); ++index)
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
// Main driver function to call all the tests
//
void TestDSPNodes()
{
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

    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::simple);
    // TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::diagonal); // ERROR: diagonal test currently broken
    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompile<float>(dsp::ConvolutionMethodOption::winograd);

    // Test simple convolution
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(8, 8, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(2, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(3, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(3, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(4, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(5, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 2, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 2, 1, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 2, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(5, 15, 4, 7, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(32, 32, 8, 8, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 1, dsp::ConvolutionMethodOption::simple);
    TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 2, dsp::ConvolutionMethodOption::simple);

    // Test unrolled convolution
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(8, 8, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(2, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(3, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(3, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(4, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(5, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 2, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 2, 1, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 2, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(5, 15, 4, 7, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(32, 32, 8, 8, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 1, dsp::ConvolutionMethodOption::unrolled);
    TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 2, dsp::ConvolutionMethodOption::unrolled);

    // Test Winograd convolution
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(8, 8, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(2, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(3, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(3, 3, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(4, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(4, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(5, 4, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 1, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(2, 2, 2, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 2, 1, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(5, 5, 1, 2, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(5, 15, 4, 7, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(32, 32, 8, 8, 3, 1, dsp::ConvolutionMethodOption::winograd);
    TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 1, dsp::ConvolutionMethodOption::winograd);
    // TestConvolutionNodeCompileVsReference<float>(120, 80, 8, 16, 3, 2, dsp::ConvolutionMethodOption::winograd); // Commented-out because Winograd doesn't support non-1 stride
}
