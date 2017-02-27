
//
// Loading map tests
//

#include "LoadMap_test.h"

// common
#include "LoadModel.h"
#include "MapLoadArguments.h"

// model
#include "Model.h"

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{
void TestLoadMapWithDefaultArgs()
{
    common::MapLoadArguments args;
    args.inputModelFilename = "../../../examples/data/model_1.model";
    args.modelInputsString = "";
    args.modelOutputsString = "1026.output"; // the LinearPredictorNode

    std::cout << "Testing map loading, file " << args.inputModelFilename << std::endl;
    auto map = common::LoadMap(args);

    // check stuff out
    std::cout << "Input[0] node id: " << map.GetInput(0)->GetId() << std::endl;
    testing::ProcessTest("Testing map load", map.GetInput(0)->Size() == 3);
    testing::ProcessTest("Testing map load", map.GetOutput(0).Size() == 1);
}

void TestLoadMapWithPorts()
{
    common::MapLoadArguments args;
    args.inputModelFilename = "../../../examples/data/model_1.model";
    args.modelInputsString = "";
    args.modelOutputsString = "{1026.weightedElements[0:2], 1026.weightedElements[4:6]}";

    std::cout << "Testing map loading, file " << args.inputModelFilename << std::endl;
    auto map = common::LoadMap(args);

    // check stuff out
    std::cout << "Input[0] node id: " << map.GetInput(0)->GetId() << std::endl;
    testing::ProcessTest("Testing map load", map.GetInput(0)->Size() == 3);
    testing::ProcessTest("Testing map load", map.GetOutput(0).Size() == 4);
}
}
