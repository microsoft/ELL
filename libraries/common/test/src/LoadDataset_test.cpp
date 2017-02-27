
//
// Loading map tests
//

#include "LoadMap_test.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{

common::DataLoadArguments GetDataLoadArguments()
{
    common::DataLoadArguments args;
    args.inputDataFilename = "../../../examples/data/testData.txt";
    return args;
}
void TestLoadDataset()
{
    auto dataLoadArguments = GetDataLoadArguments();
    auto dataset = common::GetDataset(dataLoadArguments);
}

void TestLoadMappedDataset()
{
    common::MapLoadArguments args;
    args.inputModelFilename = "../../../examples/data/model_1.model";
    args.modelInputsString = "";
    args.modelOutputsString = "1026.output";

    auto map = common::LoadMap(args);
    auto dataLoadArguments = GetDataLoadArguments();
    auto dataset = common::GetMappedDataset(dataLoadArguments, map);
}
}
