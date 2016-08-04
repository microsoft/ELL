
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
#include "Serialization.h"
#include "JsonSerializer.h"
#include "XMLSerializer.h"

// model
#include "ModelGraph.h"
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
    float b=0;
    double c=0;
    TestStruct() = default;
    TestStruct(int a, float b, double c) : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    virtual void Serialize(utilities::Serializer& serializer) const override
    {
        serializer.Serialize("a", a);
        serializer.Serialize("b", b);
        serializer.Serialize("c", c);
    }

    virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override
    {
        serializer.Deserialize("a", a, context);
        serializer.Deserialize("b", b, context);
        serializer.Deserialize("c", c, context);
    }
};

template <typename SerializerType>
void TestSerializer()
{
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

    SerializerType serializer;
    std::cout << "--Serializing int--" << std::endl;
    serializer.Serialize(intVal);
    std::cout << std::endl;

    std::cout << "--Serializing float--" << std::endl;
    serializer.Serialize(floatVal);
    std::cout << std::endl;

    std::cout << "--Serializing double--" << std::endl;
    serializer.Serialize(doubleVal);
    std::cout << std::endl;

    std::cout << "--Serializing TestStruct--" << std::endl;
    serializer.Serialize(testStruct);
    std::cout << std::endl;

    std::cout << "--Serializing UniqueId--" << std::endl;
    serializer.Serialize(id);
    std::cout << std::endl;

    std::cout << "--Serializing input node--" << std::endl;
    serializer.Serialize(*in);
    std::cout << std::endl;

    std::cout << "\n--Serializing output node--" << std::endl;
    serializer.Serialize(*out);
    std::cout << std::endl;

    std::cout << "\n--Serializing constant node--" << std::endl;
    serializer.Serialize(*constNode);
    std::cout << std::endl;

    std::cout << "\n--Serializing binary operation node--" << std::endl;
    serializer.Serialize(*binaryOpNode);
    std::cout << std::endl;

    std::cout << "\n--Serializing model--" << std::endl;
    serializer.Serialize(g);
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << std::endl;

    // simple stuff
    serializer.Serialize(5);
    std::cout << std::endl;

    serializer.Serialize(3.1415);
    std::cout << std::endl;

    std::vector<int> intArray{ 1, 2, 3 };
    serializer.Serialize("intArray", intArray);
    std::cout << std::endl;

    std::vector<bool> boolArray{ true, false, true };
    serializer.Serialize("boolArray", boolArray);
    std::cout << std::endl;

    std::vector<TestStruct> structArray;
    structArray.emplace_back(1, 2, 3);
    structArray.emplace_back(4, 5, 6);
    structArray.emplace_back(7, 8, 9);
    serializer.Serialize("structArray", structArray);
    std::cout << std::endl;
}

template <typename SerializerType, typename DeserializerType>
void TestDeserializer()
{
    utilities::SerializationContext context;

    std::cout << "Deserializer test 1" << std::endl;
    {
        std::stringstream strstream;
        SerializerType serializer(strstream);
        serializer.Serialize("pi", 3.14159);
        std::cout << "Str value: " << strstream.str() << std::endl;;

        DeserializerType deserializer(strstream);
        double val = 0;
        deserializer.Deserialize("pi", val, context);
        std::cout << "Result: " << val << std::endl;
        testing::ProcessTest("Deserialize float check", val == 3.14159);
    }
    std::cout << std::endl;

    std::cout << "Deserializer test 2" << std::endl;
    {
        std::stringstream strstream;
        SerializerType serializer(strstream);
        serializer.Serialize("pie", std::string{ "cherry pie" });
        std::cout << "Str value: " << strstream.str() << std::endl;

        DeserializerType deserializer(strstream);
        std::string val;
        deserializer.Deserialize("pie", val, context);
        std::cout << "Result: " << val << std::endl;
        testing::ProcessTest("Deserialize string check", val == "cherry pie");
    }
    std::cout << std::endl;

    std::cout << "Deserializer test 3" << std::endl;
    {
        std::stringstream strstream;
        SerializerType serializer(strstream);
        std::vector<int> arr {1,2,3};
        serializer.Serialize("arr", arr);
        std::cout << "Str value: " << strstream.str() << std::endl;

        DeserializerType deserializer(strstream);
        std::vector<int> val;
        deserializer.Deserialize("arr", val, context);
        std::cout << "Result: ";
        for(auto element: val)
            std::cout << element << ", ";
         std::cout << std::endl;
        testing::ProcessTest("Deserialize vector<int> check", val[0] == 1 && val[1] == 2 && val[2] == 3);
    }
    std::cout << std::endl;

    std::cout << "Deserializer test 4" << std::endl;
    {
        std::stringstream strstream;
        SerializerType serializer(strstream);
        TestStruct testStruct{ 1, 2.2f, 3.3 };
        serializer.Serialize("s", testStruct);
        std::cout << "Str value: " << strstream.str() << std::endl;

        DeserializerType deserializer(strstream);
        TestStruct val;
        deserializer.Deserialize("s", val, context);
        std::cout << "Result: ";
        std::cout << "a: " << val.a << ", b: " << val.b << ", c: " << val.c << std::endl;
        testing::ProcessTest("Deserialize ISerializable check",  val.a == 1 && val.b == 2.2f && val.c == 3.3);
        
    }

    std::cout << "Deserializer test 5" << std::endl;
    {
        model::Model g;
        utilities::GenericTypeFactory typeFactory;
        typeFactory.AddType<model::Node, model::InputNode<double>>();
        typeFactory.AddType<model::Node, model::OutputNode<double>>();
        typeFactory.AddType<model::Node, nodes::ConstantNode<double>>();
        typeFactory.AddType<model::Node, nodes::BinaryOperationNode<double>>();

        std::stringstream strstream;
        auto constVector = std::vector<double>{ 1.0, 2.0, 3.0 };

        SerializerType serializer(strstream);
        auto in = g.AddNode<model::InputNode<double>>(3);
        auto constNode = g.AddNode<nodes::ConstantNode<double>>(constVector);
        auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
        auto out = g.AddNode<model::OutputNode<double>>(in->output);

        serializer.Serialize("node1", *constNode);
        serializer.Serialize("node2", *in);
        serializer.Serialize("node3", constNode);
        serializer.Serialize("node4", binaryOpNode);
        std::cout << "Str value: " << "\n" << strstream.str() << std::endl;

        model::ModelSerializationContext modelContext(nullptr);
        DeserializerType deserializer(strstream);
        nodes::ConstantNode<double> newConstNode;
        model::InputNode<double> newIn;
        nodes::BinaryOperationNode<double> newBinaryOpNode;
        std::unique_ptr<nodes::ConstantNode<double>> newConstNodePtr = nullptr;
        std::unique_ptr<nodes::BinaryOperationNode<double>> newBinaryOpNodePtr = nullptr;
        deserializer.Deserialize("node1", newConstNode, modelContext);
        deserializer.Deserialize("node2", newIn, modelContext);
        deserializer.Deserialize("node3", newConstNodePtr, modelContext);

//        deserializer.Deserialize("node4", newBinaryOpNode, modelContext);

        testing::ProcessTest("Deserialize nodes check",  testing::IsEqual(constVector, newConstNode.GetValues()));
        testing::ProcessTest("Deserialize nodes check",  testing::IsEqual(constVector, newConstNodePtr->GetValues()));
    }
    std::cout << std::endl;
}

void TestJsonSerializer()
{
    TestSerializer<utilities::JsonSerializer>();
}

void TestJsonDeserializer()
{
    TestDeserializer<utilities::JsonSerializer, utilities::JsonDeserializer>();
}

void TestXmlSerializer()
{
    TestSerializer<utilities::SimpleXmlSerializer>();
}

void TestXmlDeserializer()
{
    TestDeserializer<utilities::SimpleXmlSerializer, utilities::SimpleXmlDeserializer>();
}
