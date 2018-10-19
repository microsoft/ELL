
//
// Loading map tests
//

#include "LoadMap_test.h"

// common
#include "LoadModel.h"
#include "MapLoadArguments.h"
#include "Files.h"

// model
#include "Model.h"

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{
void TestLoadMapWithDefaultArgs(const std::string& examplePath)
{
    common::MapLoadArguments args;
    args.inputModelFilename = utilities::JoinPaths(examplePath, { "models", "model_1.model" });
    args.modelInputsString = "";
    args.modelOutputsString = "1031.output"; // the LinearPredictorNode

    std::cout << "Testing map loading, file " << args.inputModelFilename << std::endl;
    auto map = common::LoadMap(args);

    // check stuff out
    std::cout << "Input[0] node id: " << map.GetInput(0)->GetId() << std::endl;
    testing::ProcessTest("Testing map load", map.GetInput(0)->Size() == 3);
    testing::ProcessTest("Testing map load", map.GetOutput(0).Size() == 1);
}

void TestLoadMapWithPorts(const std::string& examplePath)
{
    common::MapLoadArguments args;
    args.inputModelFilename = utilities::JoinPaths(examplePath, { "models", "model_1.model" });
    args.modelInputsString = "";
    args.modelOutputsString = "{1031.weightedElements[0:2], 1031.weightedElements[4:6]}";

    bool threw = false;
    std::cout << "Testing map loading, file " << args.inputModelFilename << std::endl;
    try 
    {
        auto map = common::LoadMap(args);
    }
    catch (const utilities::LogicException&)
    {
        threw = true;
    }
    // check stuff out
    testing::ProcessTest("Testing Map constructor does not support multiple output ranges", threw);
}
}
