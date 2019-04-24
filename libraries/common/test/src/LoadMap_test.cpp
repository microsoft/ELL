
//
// Loading map tests
//

#include "LoadMap_test.h"

#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>

#include <model/include/Model.h>

#include <utilities/include/Files.h>

#include <testing/include/testing.h>

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

    std::cout << "Testing map loading, file " << args.inputModelFilename << std::endl;
    auto map = common::LoadMap(args);

    // check stuff out
    testing::ProcessTest("Testing Map constructor supports multiple output ranges", map.GetOutput().Size() == 4);
}
} // namespace ell
