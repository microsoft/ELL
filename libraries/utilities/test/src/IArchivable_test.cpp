////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IArchivable_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IArchivable_test.h"

// utilities
#include "Archiver.h"
#include "IArchivable.h"
#include "JsonArchiver.h"
#include "UniqueId.h"
#include "XmlArchiver.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "ConstantNode.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <sstream>
#include <vector>

namespace ell
{
struct TestStruct : public utilities::IArchivable
{
    int a = 0;
    float b = 0.0f;
    double c = 0.0;
    TestStruct() = default;
    TestStruct(int a, float b, double c)
        : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    virtual void WriteToArchive(utilities::Archiver& archiver) const override
    {
        archiver.Archive("a", a);
        archiver.Archive("b", b);
        archiver.Archive("c", c);
    }

    virtual void ReadFromArchive(utilities::Unarchiver& archiver) override
    {
        // what about _type?
        archiver.Unarchive("a", a);
        archiver.Unarchive("b", b);
        archiver.Unarchive("c", c);
    }
};

template <typename ArchiverType>
void TestArchiver()
{
    bool boolVal = true;
    int intVal = 1;
    float floatVal = 2.5;
    double doubleVal = 3.14;
    TestStruct testStruct{ 1, 2.2f, 3.3 };

    utilities::UniqueId id;

    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto constNode = g.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, emitters::BinaryOperationType::add);
    auto out = g.AddNode<model::OutputNode<double>>(in->output);

    std::stringstream strstream;
    ArchiverType archiver(strstream);
    archiver.Archive(boolVal);

    archiver.Archive(intVal);

    archiver.Archive(floatVal);

    archiver.Archive(doubleVal);

    archiver.Archive(testStruct);

    archiver.Archive(id);

    archiver.Archive(*in);

    archiver.Archive(*out);

    archiver.Archive(*constNode);

    archiver.Archive(*binaryOpNode);

    archiver.Archive(g);

    // simple stuff
    archiver.Archive(5);

    archiver.Archive(3.1415);

    std::vector<int> intArray{ 1, 2, 3 };
    archiver.Archive("intArray", intArray);

    std::vector<bool> boolArray{ true, false, true };
    archiver.Archive("boolArray", boolArray);

    std::vector<TestStruct> structArray;
    structArray.emplace_back(1, 2.0f, 3.0);
    structArray.emplace_back(4, 5.0f, 6.0);
    structArray.emplace_back(7, 8.0f, 9.0);
    archiver.Archive("structArray", structArray);
}

template <typename ArchiverType, typename UnarchiverType>
void TestUnarchiver()
{
    utilities::SerializationContext context;

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("true", true);
        }

        UnarchiverType unarchiver(strstream, context);
        bool val = false;
        unarchiver.Unarchive("true", val);
        testing::ProcessTest("Deserialize bool check", val == true);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("pi", 3.14159);
        }

        UnarchiverType unarchiver(strstream, context);
        double val = 0;
        unarchiver.Unarchive("pi", val);
        testing::ProcessTest("Deserialize float check", val == 3.14159);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("pie", std::string{ "cherry pie" });
        }

        UnarchiverType unarchiver(strstream, context);
        std::string val;
        unarchiver.Unarchive("pie", val);
        testing::ProcessTest("Deserialize string check", val == "cherry pie");
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            std::vector<int> arr{ 1, 2, 3 };
            archiver.Archive("arr", arr);
        }

        UnarchiverType unarchiver(strstream, context);
        std::vector<int> val;
        unarchiver.Unarchive("arr", val);
        testing::ProcessTest("Deserialize vector<int> check", val[0] == 1 && val[1] == 2 && val[2] == 3);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            TestStruct testStruct{ 1, 2.2f, 3.3 };
            archiver.Archive("s", testStruct);
        }

        UnarchiverType unarchiver(strstream, context);
        TestStruct val;
        unarchiver.Unarchive("s", val);
        testing::ProcessTest("Deserialize IArchivable check", val.a == 1 && val.b == 2.2f && val.c == 3.3);
    }

    {
        model::Model g;
        utilities::SerializationContext context;
        model::ModelSerializationContext modelContext(context, &g);
        modelContext.GetTypeFactory().AddType<model::Node, model::InputNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, model::OutputNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<double>>();
        modelContext.GetTypeFactory().AddType<nodes::ConstantNode<double>, nodes::ConstantNode<double>>();

        std::stringstream strstream;
        auto constVector = std::vector<double>{ 1.0, 2.0, 3.0 };

        {
            ArchiverType archiver(strstream);
            auto in = g.AddNode<model::InputNode<double>>(3);
            auto constNode = g.AddNode<nodes::ConstantNode<double>>(constVector);
            auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, emitters::BinaryOperationType::add);
            g.AddNode<model::OutputNode<double>>(in->output);

            archiver.Archive("node1", *constNode);
            archiver.Archive("node2", *in);
            archiver.Archive("node3", constNode);
            archiver.Archive("node4", constNode);
            archiver.Archive("node5", binaryOpNode);
        }

        UnarchiverType unarchiver(strstream, context);
        unarchiver.PushContext(modelContext);
        nodes::ConstantNode<double> newConstNode;
        model::InputNode<double> newIn;
        nodes::BinaryOperationNode<double> newBinaryOpNode;
        std::unique_ptr<nodes::ConstantNode<double>> newConstNodePtr = nullptr;
        std::unique_ptr<model::Node> newNodePtr = nullptr;
        std::unique_ptr<nodes::BinaryOperationNode<double>> newBinaryOpNodePtr = nullptr;
        unarchiver.Unarchive("node1", newConstNode);
        unarchiver.Unarchive("node2", newIn);
        unarchiver.Unarchive("node3", newConstNodePtr);
        unarchiver.Unarchive("node4", newNodePtr);
        unarchiver.Unarchive("node5", newBinaryOpNode);
        unarchiver.PopContext();
        testing::ProcessTest("Deserialize nodes check", testing::IsEqual(constVector, newConstNode.GetValues()));
        testing::ProcessTest("Deserialize nodes check", testing::IsEqual(constVector, newConstNodePtr->GetValues()));
    }

    {
        // arrays of stuff
        std::stringstream strstream;
        auto doubleVector = std::vector<double>{ 1.0, 2.0, 3.0 };
        std::vector<TestStruct> structVector;
        structVector.push_back(TestStruct{ 1, 2.2f, 3.3 });
        structVector.push_back(TestStruct{ 4, 5.5f, 6.6 });

        {
            ArchiverType archiver(strstream);
            archiver.Archive("vec1", doubleVector);
            archiver.Archive("vec2", structVector);
        }

        UnarchiverType unarchiver(strstream, context);
        std::vector<double> newDoubleVector;
        std::vector<TestStruct> newStructVector;
        unarchiver.Unarchive("vec1", newDoubleVector);
        unarchiver.Unarchive("vec2", newStructVector);

        testing::ProcessTest("Deserialize array check", testing::IsEqual(doubleVector, newDoubleVector));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].a, newStructVector[0].a));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].b, newStructVector[0].b));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].c, newStructVector[0].c));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].a, newStructVector[1].a));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].b, newStructVector[1].b));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].c, newStructVector[1].c));
    }

    {
        model::Model g;
        utilities::SerializationContext context;
        model::ModelSerializationContext modelContext(context, &g);
        modelContext.GetTypeFactory().AddType<model::Node, model::InputNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, model::OutputNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<double>>();
        modelContext.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<double>>();
        modelContext.GetTypeFactory().AddType<nodes::ConstantNode<double>, nodes::ConstantNode<double>>();
        auto in = g.AddNode<model::InputNode<double>>(3);
        auto doubleVector = std::vector<double>{ 1.0, 2.0, 3.0 };
        auto constNode = g.AddNode<nodes::ConstantNode<double>>(doubleVector);
        g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, emitters::BinaryOperationType::add);
        g.AddNode<model::OutputNode<double>>(in->output);

        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);

            archiver.Archive(g);
            // std::cout << "Graph output:" << std::endl;
            // std::cout << strstream.str() << std::endl;
        }

        UnarchiverType unarchiver(strstream, context);
        unarchiver.PushContext(modelContext);
        model::Model newGraph;
        unarchiver.Unarchive(newGraph);

        std::stringstream strstream2;
        ArchiverType archiver2(strstream2);

        archiver2.Archive(newGraph);
        // std::cout << "New graph output:" << std::endl;
        // std::cout << strstream2.str() << std::endl;
    }

    {
        auto stringVal = std::string{ "Hi there! Here's a tab character: \t, as well as some 'quoted' text." };
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("str", stringVal);
        }

        UnarchiverType unarchiver(strstream, context);
        std::string val;
        unarchiver.Unarchive("str", val);
        testing::ProcessTest("Deserialize string check", val == stringVal);
    }
}

void TestJsonArchiver()
{
    TestArchiver<utilities::JsonArchiver>();
}

void TestJsonUnarchiver()
{
    TestUnarchiver<utilities::JsonArchiver, utilities::JsonUnarchiver>();
}

void TestXmlArchiver()
{
    TestArchiver<utilities::XmlArchiver>();
}

void TestXmlUnarchiver()
{
    TestUnarchiver<utilities::XmlArchiver, utilities::XmlUnarchiver>();
}
}
