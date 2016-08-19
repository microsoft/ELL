
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
    auto model1 = common::LoadModelGraph("[1]");
    auto model2 = common::LoadModelGraph("[2]");
    auto model3 = common::LoadModelGraph("[3]");

    std::cout << "Model 1 size: " << model1.Size() << std::endl;
    std::cout << "Model 2 size: " << model2.Size() << std::endl;
    std::cout << "Model 3 size: " << model3.Size() << std::endl;
}

void TestLoadTreeModels()
{
    auto model0 = common::LoadModelGraph("[tree_0]");
    auto model1 = common::LoadModelGraph("[tree_1]");
    auto model2 = common::LoadModelGraph("[tree_2]");
    auto model3 = common::LoadModelGraph("[tree_3]");

    std::cout << "Tree 0 size: " << model0.Size() << std::endl;
    std::cout << "Tree 1 size: " << model1.Size() << std::endl;
    std::cout << "Tree 2 size: " << model2.Size() << std::endl;
    std::cout << "Tree 3 size: " << model3.Size() << std::endl;
}

void TestSaveModels(std::string ext)
{   
    auto model1 = common::LoadModelGraph("[1]");
    auto model2 = common::LoadModelGraph("[2]");
    auto model3 = common::LoadModelGraph("[3]");
    auto tree0 = common::LoadModelGraph("[tree_0]");
    auto tree1 = common::LoadModelGraph("[tree_1]");
    auto tree2 = common::LoadModelGraph("[tree_2]");
    auto tree3 = common::LoadModelGraph("[tree_3]");

    common::SaveModelGraph(model1, "model_1." + ext);    
    common::SaveModelGraph(model2, "model_2." + ext);    
    common::SaveModelGraph(model3, "model_3." + ext);    
    common::SaveModelGraph(tree0, "tree_0." + ext);
    common::SaveModelGraph(tree1, "tree_1." + ext);
    common::SaveModelGraph(tree2, "tree_2." + ext);
    common::SaveModelGraph(tree3, "tree_3." + ext);    

    auto newModel1 = common::LoadModelGraph("model_1." + ext);    
    auto newModel2 = common::LoadModelGraph("model_2." + ext);    
    auto newModel3 = common::LoadModelGraph("model_3." + ext);    
    auto newTree0 = common::LoadModelGraph("tree_0." + ext);
    auto newTree1 = common::LoadModelGraph("tree_1." + ext);
    auto newTree2 = common::LoadModelGraph("tree_2." + ext);
    auto newTree3 = common::LoadModelGraph("tree_3." + ext);
}