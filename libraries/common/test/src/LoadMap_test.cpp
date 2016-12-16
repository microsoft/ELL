
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
    std::cout << "Testing map loading" << std::endl;
    common::MapLoadArguments args;
    args.inputModelFilename = "../../../examples/data/model_1.model";
    args.modelInputsString = "";
    args.modelOutputsString = "1017.output";

    auto map = common::LoadMap(args);

    // check stuff out
    std::cout << "Input[0] size: " << map.GetInput(0)->Size() << std::endl;
    std::cout << "Input[0] node id: " << map.GetInput(0)->GetId() << std::endl;
}

void TestLoadMapWithPorts()
{
    std::cout << "Testing map loading" << std::endl;
    common::MapLoadArguments args;
    args.inputModelFilename = "../../../examples/data/model_1.model";
    args.modelInputsString = "";
    args.modelOutputsString = "1017.weightedElements[0:2]";

    auto map = common::LoadMap(args);

    // check stuff out
}
}
