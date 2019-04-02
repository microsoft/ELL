////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPCodeNodesTest.cpp (nodes_test)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DSPCodeNodesTests.h"

#include "NodesTestUtilities.h"

#include <model_testing/include/ModelTestUtilities.h>

#include <common/include/DataLoaders.h>

#include <data/include/Dataset.h>
#include <data/include/Example.h>
#include <data/include/WeightLabel.h>

#include <model/include/Model.h>

#include <nodes/include/ConstantNode.h>
#include <nodes/include/GRUNode.h>
#include <nodes/include/VoiceActivityDetectorNode.h>

#include <predictors/neural/include/SigmoidActivation.h>
#include <predictors/neural/include/TanhActivation.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/StringUtil.h>

using namespace ell;
using namespace data;

template <typename ElementType>
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
    const auto& output = nodes::VoiceActivityDetector(inputNode->output, SampleRate, FrameDuration, TauUp, TauDown, LargeInput, GainAtt, ThresholdUp, ThresholdDown, LevelThreshold);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", output } });

    // Dump the module so we can debug it
    // compiledMap.GetModule().DebugDump();
    auto dataset = LoadVadData<ElementType>(path, FrameSize);

    TestWithSerialization(map, "TestVoiceActivityDetectorNode", [&dataset](model::Map& map, int iteration) {
        // compiling it.
        model::MapCompilerOptions settings;
        settings.verifyJittedModule = true;
        settings.compilerSettings.debug = true;
        settings.compilerSettings.optimize = true;
        model::IRMapCompiler compiler(settings, {});
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
    using ElementType = double;
    using VectorType = math::ColumnVector<ElementType>;

    auto dataset = LoadVadData<ElementType>(path, FrameSize);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(FrameSize);
    const auto& vadOutput = nodes::VoiceActivityDetector(inputNode->output, SampleRate, FrameDuration, TauUp, TauDown, LargeInput, GainAtt, ThresholdUp, ThresholdDown, LevelThreshold);

    size_t inputSize = FrameSize;
    size_t hiddenUnits = 10;
    size_t stackSize = 3; // GRU stacks the 3 weights for input, reset, hidden into one matrix.
    size_t numRows = hiddenUnits * stackSize;
    size_t numCols = inputSize;
    VectorType inputWeightsVector(std::vector<ElementType>(numRows * numCols, static_cast<ElementType>(0.01)));
    numCols = hiddenUnits;
    VectorType hiddenWeightsVector(std::vector<ElementType>(numRows * numCols, static_cast<ElementType>(0.02)));

    VectorType inputBiasVector(std::vector<ElementType>(numRows, static_cast<ElementType>(0.01)));
    VectorType hiddenBiasVector(std::vector<ElementType>(numRows, static_cast<ElementType>(0.02)));

    const auto& inputWeights = nodes::Constant(model, inputWeightsVector.ToArray());
    const auto& hiddenWeights = nodes::Constant(model, hiddenWeightsVector.ToArray());
    const auto& inputBias = nodes::Constant(model, inputBiasVector.ToArray());
    const auto& hiddenBias = nodes::Constant(model, hiddenBiasVector.ToArray());

    auto gruNode = model.AddNode<nodes::GRUNode<ElementType>>(inputNode->output, vadOutput, hiddenUnits, inputWeights, hiddenWeights, inputBias, hiddenBias, ell::predictors::neural::Activation<ElementType>(new ell::predictors::neural::TanhActivation<ElementType>()), ell::predictors::neural::Activation<ElementType>(new ell::predictors::neural::SigmoidActivation<ElementType>()));

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", gruNode->output } });

    TestWithSerialization(map, "TestGRUNodeWithVADReset", [&dataset, hiddenUnits](model::Map& map, int iteration) {
        // now test compiling it.
        model::MapCompilerOptions settings;
        settings.verifyJittedModule = true;
        settings.compilerSettings.optimize = false;
        settings.compilerSettings.debug = true;
        model::IRMapCompiler compiler(settings, {});
        auto compiledMap = compiler.Compile(map);
        int errors = 0;

        // Now since the model is compiled, in order to observe the hidden state being reset we
        // need to access the global variable for that, which in this case will be called "g_1".
        model::IRCompiledMap* icmap = dynamic_cast<model::IRCompiledMap*>(&compiledMap);

        size_t numFrames = dataset.NumExamples();
        int lastSignal = 0;
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

            // compute the sum of the hidden state.
            ElementType* g_1 = icmap->GetGlobalValuePointer<ElementType>("g_1");
            ElementType hiddenSum = 0;
            for (size_t i = 0; i < hiddenUnits; i++)
            {
                ElementType v = g_1[i];
                hiddenSum += v;
            }

            // Now when the VAD signal transitions from 1 to zero, the GRU node should be reset
            if (lastSignal == 1 && expectedSignal == 0)
            {
                // reset should have happened which means the GRU hidden state should be zero.
                if (hiddenSum != 0)
                {
                    // hidden state was not reset!
                    errors++;
                }
            }

            // this is handy for debugging, graph this output to see the result of the GRU reset.
            // std::cout << expectedSignal << "," << hiddenSum << ", ";
            lastSignal = expectedSignal;
        }
        std::cout << "\n";
        testing::ProcessTest(utilities::FormatString("Testing TestGRUNodeWithVADReset iteration %d, %d errors", iteration, errors), errors == 0);
    });
}

// Main driver function to call all the tests
//
void TestDSPCodeNodes(const std::string& path)
{
    TestVoiceActivityDetectorNode(path);
    TestGRUNodeWithVADReset(path);
}
