
//
// Loading model tests
//

#include "LoadModelGraph_test.h"

// common
#include "LoadModelGraph.h"

// model
#include "ModelGraph.h"

// testing
#include "testing.h"

// stl
#include <iostream>


void TestLoadSampleModels()
{
    auto model1 = common::LoadModelGraph("1");
    auto model2 = common::LoadModelGraph("2");
    auto model3 = common::LoadModelGraph("3");

    std::cout << "Model 1 size: " << model1.Size() << std::endl;
    std::cout << "Model 2 size: " << model2.Size() << std::endl;
    std::cout << "Model 3 size: " << model3.Size() << std::endl;
}

void TestLoadTreeModels()
{
    auto model0 = common::LoadModelGraph("tree_0");
    auto model1 = common::LoadModelGraph("tree_1");
    auto model2 = common::LoadModelGraph("tree_2");
    auto model3 = common::LoadModelGraph("tree_3");

    std::cout << "Model 0 size: " << model0.Size() << std::endl;
    std::cout << "Model 1 size: " << model1.Size() << std::endl;
    std::cout << "Model 2 size: " << model2.Size() << std::endl;
    std::cout << "Model 3 size: " << model3.Size() << std::endl;
}

