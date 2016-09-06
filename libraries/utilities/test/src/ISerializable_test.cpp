////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable_test.h"

// utilities
#include "UniqueId.h"
#include "ISerializable.h"
#include "Archiver.h"
#include "JsonArchiver.h"
#include "XMLArchiver.h"

// model
#include "Model.h"
#include "InputNode.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "ConstantNode.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <vector>
#include <sstream>

struct TestStruct : public utilities::ISerializable
{
    int a=0;
    float b=0.0f;
    double c=0.0;
    TestStruct() = default;
    TestStruct(int a, float b, double c) : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    virtual void Serialize(utilities::Archiver& archiver) const override
    {
        archiver.Serialize("a", a);
        archiver.Serialize("b", b);
        archiver.Serialize("c", c);
    }

    virtual void Deserialize(utilities::Unarchiver& archiver) override
    {
        // what about _type?
        archiver.Deserialize("a", a);
        archiver.Deserialize("b", b);
        archiver.Deserialize("c", c);
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
    auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
    auto out = g.AddNode<model::OutputNode<double>>(in->output);


    std::stringstream strstream;
    ArchiverType archiver(strstream);
    archiver.Serialize(boolVal);

    archiver.Serialize(intVal);

    archiver.Serialize(floatVal);

    archiver.Serialize(doubleVal);

    archiver.Serialize(testStruct);

    archiver.Serialize(id);

    archiver.Serialize(*in);

    archiver.Serialize(*out);

    archiver.Serialize(*constNode);

    archiver.Serialize(*binaryOpNode);

    archiver.Serialize(g);

    // simple stuff
    archiver.Serialize(5);

    archiver.Serialize(3.1415);

    std::vector<int> intArray{ 1, 2, 3 };
    archiver.Serialize("intArray", intArray);

    std::vector<bool> boolArray{ true, false, true };
    archiver.Serialize("boolArray", boolArray);

    std::vector<TestStruct> structArray;
    structArray.emplace_back(1, 2.0f, 3.0);
    structArray.emplace_back(4, 5.0f, 6.0);
    structArray.emplace_back(7, 8.0f, 9.0);
    archiver.Serialize("structArray", structArray);
}

template <typename ArchiverType, typename UnarchiverType>
void TestUnarchiver()
{
    utilities::SerializationContext context;

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Serialize("true", true);
        }

        UnarchiverType deserializer(strstream, context);
        bool val = false;
        deserializer.Deserialize("true", val);
        testing::ProcessTest("Deserialize bool check", val == true);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Serialize("pi", 3.14159);
        }

        UnarchiverType deserializer(strstream, context);
        double val = 0;
        deserializer.Deserialize("pi", val);
        testing::ProcessTest("Deserialize float check", val == 3.14159);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Serialize("pie", std::string{ "cherry pie" });
        }

        UnarchiverType deserializer(strstream, context);
        std::string val;
        deserializer.Deserialize("pie", val);
        testing::ProcessTest("Deserialize string check", val == "cherry pie");
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            std::vector<int> arr{ 1,2,3 };
            archiver.Serialize("arr", arr);
        }

        UnarchiverType deserializer(strstream, context);
        std::vector<int> val;
        deserializer.Deserialize("arr", val);
        testing::ProcessTest("Deserialize vector<int> check", val[0] == 1 && val[1] == 2 && val[2] == 3);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            TestStruct testStruct{ 1, 2.2f, 3.3 };
            archiver.Serialize("s", testStruct);
        }

        UnarchiverType deserializer(strstream, context);
        TestStruct val;
        deserializer.Deserialize("s", val);
        testing::ProcessTest("Deserialize ISerializable check",  val.a == 1 && val.b == 2.2f && val.c == 3.3);        
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
            auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
            auto out = g.AddNode<model::OutputNode<double>>(in->output);

            archiver.Serialize("node1", *constNode);
            archiver.Serialize("node2", *in);
            archiver.Serialize("node3", constNode);
            archiver.Serialize("node4", constNode);
            archiver.Serialize("node5", binaryOpNode);
        }

        UnarchiverType deserializer(strstream, context);
        deserializer.PushContext(modelContext);
        nodes::ConstantNode<double> newConstNode;
        model::InputNode<double> newIn;
        nodes::BinaryOperationNode<double> newBinaryOpNode;
        std::unique_ptr<nodes::ConstantNode<double>> newConstNodePtr = nullptr;
        std::unique_ptr<model::Node> newNodePtr = nullptr;
        std::unique_ptr<nodes::BinaryOperationNode<double>> newBinaryOpNodePtr = nullptr;
        deserializer.Deserialize("node1", newConstNode);
        deserializer.Deserialize("node2", newIn);
        deserializer.Deserialize("node3", newConstNodePtr);
        deserializer.Deserialize("node4", newNodePtr);
        deserializer.Deserialize("node5", newBinaryOpNode);
        deserializer.PopContext();
        testing::ProcessTest("Deserialize nodes check",  testing::IsEqual(constVector, newConstNode.GetValues()));
        testing::ProcessTest("Deserialize nodes check",  testing::IsEqual(constVector, newConstNodePtr->GetValues()));
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
            archiver.Serialize("vec1", doubleVector);
            archiver.Serialize("vec2", structVector);
        }

        UnarchiverType deserializer(strstream, context);
        std::vector<double> newDoubleVector;
        std::vector<TestStruct> newStructVector;
        deserializer.Deserialize("vec1", newDoubleVector);
        deserializer.Deserialize("vec2", newStructVector);

        testing::ProcessTest("Deserialize array check",  testing::IsEqual(doubleVector, newDoubleVector));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[0].a, newStructVector[0].a));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[0].b, newStructVector[0].b));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[0].c, newStructVector[0].c));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[1].a, newStructVector[1].a));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[1].b, newStructVector[1].b));
        testing::ProcessTest("Deserialize array check",  testing::IsEqual(structVector[1].c, newStructVector[1].c));
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
        auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
        auto out = g.AddNode<model::OutputNode<double>>(in->output);

        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);

            archiver.Serialize(g);
            // std::cout << "Graph output:" << std::endl;
            // std::cout << strstream.str() << std::endl;
        }

        UnarchiverType deserializer(strstream, context);
        deserializer.PushContext(modelContext);
        model::Model newGraph;
        deserializer.Deserialize(newGraph);

        std::stringstream strstream2;
        ArchiverType archiver2(strstream2);

        archiver2.Serialize(newGraph);
        // std::cout << "New graph output:" << std::endl;
        // std::cout << strstream2.str() << std::endl;
    }

    {
        auto stringVal = std::string{ "Hi there! Here's a tab character: \t, as well as some 'quoted' text." };
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Serialize("str", stringVal);
        }
        
        UnarchiverType deserializer(strstream, context);
        std::string val;
        deserializer.Deserialize("str", val);
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
    TestArchiver<utilities::SimpleXmlArchiver>();
}

void TestXmlUnarchiver()
{
    TestUnarchiver<utilities::SimpleXmlArchiver, utilities::SimpleXmlUnarchiver>();
}
