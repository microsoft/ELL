
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
    int a = 1;
    float b = 2.5;
    double c = 1.41;

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
};


void TestISerializable()
{
    int intVal = 1;
    float floatVal = 2.5;
    double doubleVal = 3.14;
    TestStruct testStruct;
    
    utilities::UniqueId id;

    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto constNode = g.AddNode<nodes::ConstantNode<double>>(std::vector<double>{1.0, 2.0, 3.0});
    auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
    auto out = g.AddNode<model::OutputNode<double>>(in->output);

    utilities::SimpleSerializer serializer;
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
}
