////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelBuilder_test.h"
#include "ModelTestUtilities.h"

// common
#include "RegisterNodeCreators.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "ModelBuilder.h"
#include "OutputNode.h"
#include "PortElements.h"

// nodes
#include "ExtremalValueNode.h"
#include "MovingAverageNode.h"

// testing
#include "testing.h"

// utilities
#include "FunctionUtils.h"
#include "Variant.h"

// stl
#include <tuple>

using namespace ell;

//
// Tests
//
void TestTemplateAddNode()
{
    // This just tests the ModelBuilder's type-safe forwarding of AddNode to Model::AddNode
    model::Model model;
    model::ModelBuilder mb;
    auto in = mb.AddNode<model::InputNode<double>>(model, 3);
    auto minAndArgMin = mb.AddNode<nodes::ArgMinNode<double>>(model, in->output);
    auto maxAndArgMax = mb.AddNode<nodes::ArgMaxNode<double>>(model, in->output);
    auto meanMin = mb.AddNode<nodes::MovingAverageNode<double>>(model, minAndArgMin->val, 2);
    auto meanMax = mb.AddNode<nodes::MovingAverageNode<double>>(model, maxAndArgMax->val, 2);
    mb.AddNode<model::OutputNode<double>>(model, model::PortElements<double>({ meanMin->output, meanMax->output }));
    testing::ProcessTest("ModelBuilder::AddNode<>", model.Size() == 6);
}

//
// GetArgumentTypes tests
//

// InputNode
void TestInputNodeGetArgumentTypes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    // InputNode<double>
    auto inputCreatorTypes = mb.GetAddNodeArgs("InputNode<double>");
    testing::ProcessTest("Get InputNode<double> creator arguments size", inputCreatorTypes.size() == 1);
    if (inputCreatorTypes.size() == 1)
    {
        testing::ProcessTest("Get InputNode<double> creator arguments type", inputCreatorTypes[0].IsType<size_t>());
    }

    // now InputNode<int>
    inputCreatorTypes = mb.GetAddNodeArgs("InputNode<int>");
    testing::ProcessTest("Get InputNode<int> creator arguments size", inputCreatorTypes.size() == 1);
    if (inputCreatorTypes.size() == 1)
    {
        testing::ProcessTest("Get InputNode<int> creator arguments type", inputCreatorTypes[0].IsType<size_t>());
    }
}

// ConstantNode
void TestConstantNodeGetArgumentTypes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    // ConstantNode<double>
    auto constantCreatorTypes = mb.GetAddNodeArgs("ConstantNode<double>");
    testing::ProcessTest("Get ConstantNode<double> creator arguments size", constantCreatorTypes.size() == 1);
    if (constantCreatorTypes.size() == 1)
    {
        testing::ProcessTest("Get ConstantNode<double> creator arguments type", constantCreatorTypes[0].IsType<std::vector<double>>());
    }

    // now ConstantNode<int>
    constantCreatorTypes = mb.GetAddNodeArgs("ConstantNode<int>");
    testing::ProcessTest("Get ConstantNode<int> creator arguments size", constantCreatorTypes.size() == 1);
    if (constantCreatorTypes.size() == 1)
    {
        testing::ProcessTest("Get ConstantNode<int> creator arguments type", constantCreatorTypes[0].IsType<std::vector<int>>());
    }
}

// OutputNode
void TestOutputNodeGetArgumentTypes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    // OutputNode<double>
    auto creatorTypes = mb.GetAddNodeArgs("OutputNode<double>");
    testing::ProcessTest("Get OutputNode<double> creator arguments size", creatorTypes.size() == 1);
    if (creatorTypes.size() == 1)
    {
        testing::ProcessTest("Get OutputNode<double> creator arguments type", creatorTypes[0].IsType<model::PortElementsProxy>());
    }

    // now OutputNode<int>
    creatorTypes = mb.GetAddNodeArgs("OutputNode<int>");
    testing::ProcessTest("Get OutputNode<int> creator arguments size", creatorTypes.size() == 1);
    if (creatorTypes.size() == 1)
    {
        testing::ProcessTest("Get OutputNode<int> creator arguments type", creatorTypes[0].IsType<model::PortElementsProxy>());
    }
}

// BinaryOpNode
void TestBinaryOpNodeGetArgumentTypes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    // BinaryOpNode<double>
    auto creatorTypes = mb.GetAddNodeArgs("BinaryOperationNode<double>");
    testing::ProcessTest("Get BinaryOperationNode<double> creator arguments size", creatorTypes.size() == 3);
    if (creatorTypes.size() == 1)
    {
        testing::ProcessTest("Get BinaryOpNode<double> creator arguments type", creatorTypes[0].IsType<model::PortElementsProxy>());
    }

    // now BinaryOpNode<int>
    creatorTypes = mb.GetAddNodeArgs("BinaryOperationNode<int>");
    testing::ProcessTest("Get BinaryOperationNode<int> creator arguments size", creatorTypes.size() == 3);
    if (creatorTypes.size() == 1)
    {
        testing::ProcessTest("Get BinaryOpNode<int> creator arguments type", creatorTypes[0].IsType<model::PortElementsProxy>());
    }
}

//
// The following tests check AddNode(string, vector<Variant>), where variants have the correct type
//

// InputNode
void TestVariantAddInputNodes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto in = mb.AddNode(model, "InputNode<double>", { utilities::Variant(size_t(3)) });
    testing::ProcessTest("AddNode(string)", in != nullptr);

    auto in2 = mb.AddNode(model, "InputNode<int>", { utilities::Variant(size_t(3)) });
    testing::ProcessTest("AddNode(string) 2", in2 != nullptr);

    testing::ProcessTest("Model size", model.Size() == 2);
}

// ConstantNode
void TestVariantAddConstantNodes()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto in = mb.AddNode(model, "ConstantNode<double>", { utilities::Variant(std::vector<double>{ 1.0, 2.0, 3.0 }) });
    testing::ProcessTest("AddNode(string)", in != nullptr);

    auto in2 = mb.AddNode(model, "ConstantNode<int>", { utilities::Variant(std::vector<int>{ 1, 2, 3 }) });
    testing::ProcessTest("AddNode(string) 2", in2 != nullptr);
    testing::ProcessTest("Model size", model.Size() == 2);
}

// OutputNode
void TestVariantAddOutputNode()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto inputNode = mb.AddNode(model, "InputNode<double>", { utilities::Variant(size_t(3)) });
    testing::ProcessTest("AddNode(string)", inputNode != nullptr);

    model::PortElementsProxy proxy(model::PortRangeProxy{ inputNode->GetId(), "output", model::Port::PortType::real, 0, 3 });
    auto outputNode = mb.AddNode(model, "OutputNode<double>", { utilities::Variant(proxy) });
    testing::ProcessTest("AddNode(string)", outputNode != nullptr);

    testing::ProcessTest("Model size", model.Size() == 2);
}

// BinaryOpNode
void TestVariantAddBinaryOpNode()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto in = mb.AddNode(model, "InputNode<double>", { utilities::Variant(size_t(3)) });
    testing::ProcessTest("AddNode(string)", in != nullptr);

    model::PortElementsProxy proxy(model::PortRangeProxy{ in->GetId(), "output", model::Port::PortType::real, 0, 3 });
    auto op = emitters::BinaryOperationType::add;
    auto out = mb.AddNode(model, "BinaryOperationNode<double>", { utilities::Variant(proxy), utilities::Variant(proxy), utilities::Variant(op) });
    testing::ProcessTest("AddNode(string)", out != nullptr);

    testing::ProcessTest("Model size", model.Size() == 2);
}

//
// AddNode(string, vector<Variant>) where variants are convertable to the right type
//
void TestVariantAddInputNodesConvertableArgs()
{
    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto in = mb.AddNode(model, "InputNode<double>", { utilities::Variant(3) });
    testing::ProcessTest("AddNode(string)", in != nullptr);

    auto in2 = mb.AddNode(model, "InputNode<int>", { utilities::Variant(3) });
    testing::ProcessTest("AddNode(string) 2", in2 != nullptr);

    testing::ProcessTest("Model size", model.Size() == 2);
}

//
// Parsed args
//
void TestVariantAddInputNodesParsedArgs()
{
    using utilities::to_string;

    model::Model model;
    model::ModelBuilder mb;
    common::RegisterNodeCreators(mb);

    auto doubleInputNode = mb.AddNode(model, "InputNode<double>", { "3" });
    testing::ProcessTest("AddNode(InputNode<double>, string)", doubleInputNode != nullptr);

    auto intInputNode = mb.AddNode(model, "InputNode<int>", { "3" });
    testing::ProcessTest("AddNode(InputNode<int>, string)", intInputNode != nullptr);

    std::string doubleNodeIdStr = to_string(doubleInputNode->GetId());
    auto doubleOutputNode = mb.AddNode(model, "OutputNode<double>", { doubleNodeIdStr + ".output[1:2]" });
    testing::ProcessTest("AddNode(OutputNode<double>, string)", doubleOutputNode != nullptr);

    std::string intNodeIdStr = to_string(intInputNode->GetId());
    auto intOutputNode = mb.AddNode(model, "OutputNode<int>", { intNodeIdStr + ".output" });
    testing::ProcessTest("AddNode(OutputNode<int>, string)", intOutputNode != nullptr);

    testing::ProcessTest("Model size", model.Size() == 4);
}
