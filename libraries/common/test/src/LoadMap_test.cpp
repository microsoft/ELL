
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

namespace emll
{
void TestLoadMapWithDefaultArgs()
{
    std::cout << "Testing map loading" << std::endl;
    common::MapLoadArguments args;
    args.inputModelFile = "model_1.xml";
    args.modelInputsString = "";
    args.modelOutputsString = "1377.output";

    auto map = common::LoadMap(args);

    // check stuff out
    std::cout << "Input[0] size: " << map.GetInput(0)->Size() << std::endl;
    std::cout << "Input[0] node id: " << map.GetInput(0)->GetId() << std::endl;
}

void TestLoadMapWithPorts()
{
    std::cout << "Testing map loading" << std::endl;
    common::MapLoadArguments args;
    args.inputModelFile = "model_1.xml";
    args.modelInputsString = "";
    args.modelOutputsString = "1377.weightedElements[0:2]";

    auto map = common::LoadMap(args);

    // check stuff out
}
}
