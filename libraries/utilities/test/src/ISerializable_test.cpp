
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

void TestISerializable()
{
    utilities::SimpleSerializer serializer;

    utilities::UniqueId id;
    std::cout << "--Serializing UniqueId--" << std::endl;
    serializer.Serialize(id);

    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto constNode = g.AddNode<nodes::ConstantNode<double>>(std::vector<double>{1.0, 2.0, 3.0});
    auto binaryOpNode = g.AddNode<nodes::BinaryOperationNode<double>>(in->output, constNode->output, nodes::BinaryOperationNode<double>::OperationType::add);
    auto out = g.AddNode<model::OutputNode<double>>(in->output);

    std::cout << "--Serializing input node--" << std::endl;
    serializer.Serialize(*in);

    std::cout << "\n--Serializing output node--" << std::endl;
    serializer.Serialize(*out);

    std::cout << "\n--Serializing constant node--" << std::endl;
    serializer.Serialize(*constNode);

    std::cout << "\n--Serializing binary operation node--" << std::endl;
    serializer.Serialize(*binaryOpNode);
}
