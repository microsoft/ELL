////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestModelOutputDataCache.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestModelOutputDataCache.h"

#include "DataUtils.h"
#include "ModelOutputDataCache.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>

#include <nodes/include/UnaryOperationNode.h>

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>

// stl
#include <algorithm>
#include <iostream>
#include <numeric>

using namespace ell;
using namespace ell::model;
using namespace ell::testing;

namespace
{
UnlabeledDataContainer GetTestDataset()
{
    UnlabeledDataContainer result;
    std::vector<float> row(10);

    const int numRows = 12;
    for (int i = 0; i < numRows; ++i)
    {
        std::iota(row.begin(), row.end(), static_cast<float>(i));
        result.Add({ row });
    }

    return result;
}

Model GetLinearTestModel()
{
    //  in -> sqrt -> square
    Model model;
    auto in = model.AddNode<InputNode<float>>(10);
    auto n1 = model.AddNode<nodes::UnaryOperationNode<float>>(in->output, nodes::UnaryOperationType::sqrt);
    auto n2 = model.AddNode<nodes::UnaryOperationNode<float>>(n1->output, nodes::UnaryOperationType::square);
    auto n3 = model.AddNode<nodes::UnaryOperationNode<float>>(n2->output, nodes::UnaryOperationType::sqrt);
    auto n4 = model.AddNode<nodes::UnaryOperationNode<float>>(n3->output, nodes::UnaryOperationType::square);
    auto n5 = model.AddNode<nodes::UnaryOperationNode<float>>(n4->output, nodes::UnaryOperationType::sqrt);
    auto n6 = model.AddNode<nodes::UnaryOperationNode<float>>(n5->output, nodes::UnaryOperationType::square);
    auto n7 = model.AddNode<nodes::UnaryOperationNode<float>>(n6->output, nodes::UnaryOperationType::sqrt);
    auto n8 = model.AddNode<nodes::UnaryOperationNode<float>>(n7->output, nodes::UnaryOperationType::square);
    auto n9 = model.AddNode<nodes::UnaryOperationNode<float>>(n8->output, nodes::UnaryOperationType::sqrt);
    auto n10 = model.AddNode<nodes::UnaryOperationNode<float>>(n9->output, nodes::UnaryOperationType::square);
    model.AddNode<OutputNode<float>>(n10->output);
    return model;
}

std::vector<const OutputPortBase*> GetModelOutputPorts(const Model& model)
{
    std::vector<const OutputPortBase*> result;
    auto it = model.GetNodeIterator();
    while (it.IsValid())
    {
        auto node = it.Get();
        for (auto p : node->GetOutputPorts())
        {
            result.push_back(p);
        }
        it.Next();
    }
    return result;
}
} // namespace

void TestModelOutputDataCache()
{
    FailOnException(TestModelOutputDataCache_CreateAndPopulate);
    FailOnException(TestModelOutputDataCache_FindNearestCachedOutput);
}

void TestModelOutputDataCache_CreateAndPopulate()
{
    auto model = GetLinearTestModel();
    auto data = GetTestDataset();
    auto outputPorts = GetModelOutputPorts(model);
    ProcessTest("Checking output ports test data", outputPorts.size() == 12);

    ModelOutputDataCache cache;

    ProcessTest("Testing HasCacheData on new ModelOutputDataCache",
                std::all_of(outputPorts.begin(), outputPorts.end(), [&cache](auto p) {
                    return !cache.HasCachedData(p);
                }));

    cache.SetCachedData(outputPorts[0], data);
    ProcessTest("Testing HasCacheData after adding an entry",
                std::all_of(outputPorts.begin(), outputPorts.end(), [&cache, &outputPorts](auto p) {
                    return cache.HasCachedData(p) == (p == outputPorts[0]);
                }));

    cache.SetCachedData(outputPorts[2], data);
    ProcessTest("Testing HasCacheData after adding another entry",
                std::all_of(outputPorts.begin(), outputPorts.end(), [&cache, &outputPorts](auto p) {
                    return cache.HasCachedData(p) == (p == outputPorts[0] || p == outputPorts[2]);
                }));

    cache.RemoveCachedData(outputPorts[2]);
    ProcessTest("Testing HasCacheData after removing an entry",
                std::all_of(outputPorts.begin(), outputPorts.end(), [&cache, &outputPorts](auto p) {
                    return cache.HasCachedData(p) == (p == outputPorts[0]);
                }));

    ProcessTest("Testing GetCachedData", cache.GetCachedData(outputPorts[0])[1] == data[1]);

    cache.RemoveCachedData(outputPorts[0]);
    ProcessTest("Testing HasCacheData after removing all entries",
                std::all_of(outputPorts.begin(), outputPorts.end(), [&cache](auto p) {
                    return !cache.HasCachedData(p);
                }));
}

void TestModelOutputDataCache_FindNearestCachedOutput()
{
    auto model = GetLinearTestModel();
    auto data = GetTestDataset();
    auto outputPorts = GetModelOutputPorts(model);
    ProcessTest("Checking output ports test data", outputPorts.size() == 12);

    ModelOutputDataCache cache;
    cache.SetCachedData(outputPorts[0], data);
    cache.SetCachedData(outputPorts[1], data);

    ProcessTest("Testing FindNearestCachedOutput", cache.FindNearestCachedOutputPort(outputPorts[3]) == outputPorts[1]);
}
