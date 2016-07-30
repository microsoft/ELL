
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable_test.h"

// utilities
#include "Variant.h"
#include "ISerializable.h"
#include "UniqueId.h"
#include "Serialization.h"
#include "SimpleJsonSerializer.h"

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

struct TestStruct : public utilities::ISerializable, public utilities::IDescribable
{
    int a;
    float b;
    double c;
    TestStruct(int a, float b, double c) : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        auto result = utilities::ObjectDescription::FromType(*this);
        result.AddField("a", a);
        result.AddField("b", b);
        result.AddField("c", c);
        return result;
    }

    virtual void Serialize(utilities::Serializer& serializer) const override
    {
        serializer.Serialize("a", a);
        serializer.Serialize("b", b);
        serializer.Serialize("c", c);
    }
};

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

    utilities::SimpleJsonSerializer serializer;
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

void TestDeserializer()
{
    std::cout << "Deserializer test 1" << std::endl;
    {
        std::stringstream strstream;
        utilities::SimpleJsonSerializer serializer(strstream);
        serializer.Serialize("pi", 3.14159);
        std::cout << "Str value: " << strstream.str() << std::endl;;

        utilities::SimpleJsonDeserializer deserializer(strstream);
        double val = 0;
        deserializer.Deserialize("pi", val);
        std::cout << "Result: " << val << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Deserializer test 2" << std::endl;
    {
        std::stringstream strstream;
        utilities::SimpleJsonSerializer serializer(strstream);
        serializer.Serialize("pie", std::string{ "cherry" });
        std::cout << "Str value: " << strstream.str() << std::endl;

        utilities::SimpleJsonDeserializer deserializer(strstream);
        std::string val;
        deserializer.Deserialize("pie", val);
        std::cout << "Result: " << val << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Deserializer test 3" << std::endl;
    {
        std::stringstream strstream;
        utilities::SimpleJsonSerializer serializer(strstream);
        std::vector<int> arr {1,2,3};
        serializer.Serialize("arr", arr);
        std::cout << "Str value: " << strstream.str() << std::endl;

        utilities::SimpleJsonDeserializer deserializer(strstream);
        std::vector<int> val;
        deserializer.Deserialize("arr", val);
        std::cout << "Result: ";
        for(auto element: val)
            std::cout << element << ", ";
         std::cout << std::endl;
    }
    std::cout << std::endl;
}
