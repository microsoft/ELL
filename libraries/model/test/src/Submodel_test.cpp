////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Submodel_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Submodel_test.h"

#include <model_testing/include/ModelTestUtilities.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>
#include <model/include/Submodel.h>

#include <testing/include/testing.h>

#include <set>

using namespace ell;
using namespace ell::model;

void TestSubmodels()
{
    TestSubmodelConstructors();
    TestSubmodelVisit();
}

void TestSubmodelConstructors()
{
    auto model = GetSimpleModel();
    int modelSize = static_cast<int>(model.Size());
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();

    {
        Submodel submodel{ model };
        testing::ProcessTest("Testing Submodel(const Model&)", testing::IsEqual(modelSize, submodel.Size()));
    }

    {
        std::vector<const OutputPortBase*> outputs{ &outputNodes[0]->input.GetReferencedPort() };
        Submodel submodel{ outputs };
        testing::ProcessTest("Testing Submodel(const std::vector<const OutputPortBase*>&)", testing::IsEqual(modelSize - 1, submodel.Size()));
    }
}

void TestSubmodelVisit()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();

    {
        Submodel submodel{ model };
        int count = 0;
        std::set<utilities::UniqueId> visitedNodes;
        submodel.Visit([&count, &visitedNodes](const model::Node& n) {
            ++count;
            visitedNodes.insert(n.GetId());
        });
        testing::ProcessTest("Testing Submodel::Visit", testing::IsEqual(count, submodel.Size()) && testing::IsEqual(static_cast<int>(visitedNodes.size()), count));
    }
}
