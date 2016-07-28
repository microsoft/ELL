
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

struct TestStruct : public utilities::ISerializable
{
    int a;
    float b;
    double c;
    TestStruct(int a, float b, double c) : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

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

void TestISerializable()
{
    //int intVal = 1;
    //float floatVal = 2.5;
    //double doubleVal = 3.14;
    //TestStruct testStruct{ 1, 2.2f, 3.3 };

    //utilities::UniqueId id;

    //model::Model g;
    //auto in = g.AddNode<model::InputNode<double>>(3);
    //auto constNode = g.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    //auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
    //auto out = g.AddNode<model::OutputNode<double>>(in->output);

    //utilities::SimpleSerializer2 serializer;
    //std::cout << "--Serializing int--" << std::endl;
    //serializer.Serialize(intVal);
    //std::cout << std::endl;

    //std::cout << "--Serializing float--" << std::endl;
    //serializer.Serialize(floatVal);
    //std::cout << std::endl;

    //std::cout << "--Serializing double--" << std::endl;
    //serializer.Serialize(doubleVal);
    //std::cout << std::endl;

    //std::cout << "--Serializing TestStruct--" << std::endl;
    //serializer.Serialize(testStruct);
    //std::cout << std::endl;

    //std::cout << "--Serializing UniqueId--" << std::endl;
    //serializer.Serialize(id);
    //std::cout << std::endl;

    //std::cout << "--Serializing input node--" << std::endl;
    //serializer.Serialize(*in);
    //std::cout << std::endl;

    //std::cout << "\n--Serializing output node--" << std::endl;
    //serializer.Serialize(*out);
    //std::cout << std::endl;

    //std::cout << "\n--Serializing constant node--" << std::endl;
    //serializer.Serialize(*constNode);
    //std::cout << std::endl;

    //std::cout << "\n--Serializing binary operation node--" << std::endl;
    //serializer.Serialize(*binaryOpNode);
    //std::cout << std::endl;

    //std::cout << "\n--Serializing model--" << std::endl;
    //serializer.Serialize(g);
    //std::cout << std::endl;
}






void TestSerializer()
{
    utilities::SimpleSerializer serializer;

    serializer.Serialize("five", 5);
    serializer.Serialize("pi", 3.1415);

    TestStruct testStruct{ 1, 2.2f, 3.3 };
    serializer.Serialize("testStruct", testStruct);

    std::vector<int> intArray{ 1, 2, 3 };
    serializer.Serialize("intArray", intArray);

    std::vector<bool> boolArray{ true, false, true };
    serializer.Serialize("boolArray", boolArray);

    std::vector<TestStruct> structArray;
    structArray.emplace_back(1, 2, 3);
    structArray.emplace_back(4, 5, 6);
    structArray.emplace_back(7, 8, 9);
//    serializer.Serialize("structArray", structArray);

    utilities::foo scalarFoo; // weird struct
    std::vector<utilities::foo> vectorFoo;
//    serializer.Serialize("scalarFoo", scalarFoo);
//    serializer.Serialize("vectorFoo", vectorFoo);

//    serializer.Foo(vectorFoo);


    bool value1 = utilities::is_vector<int>::value;
    bool value2 = utilities::is_vector<std::vector<int>>::value;
    std::cout << value1 << ", " << value2 << std::endl;
}
