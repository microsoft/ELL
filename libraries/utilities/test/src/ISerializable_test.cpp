
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

// model
#include "ModelGraph.h"
#include "InputNode.h"
#include "OutputNode.h"

// testing
#include "testing.h"

// stl
#include <iostream>

void TestISerializable()
{
    utilities::SimpleSerializer serializer;

    // model::Model g;
    // auto in = g.AddNode<model::InputNode<double>>(3);
    // auto out = g.AddNode<model::OutputNode<double>>(in->output);

    // std::cout << "--Serializing input node--" << std::endl;
    // serializer.Serialize(*in);

    // std::cout << "\n--Serializing output node--" << std::endl;
    // serializer.Serialize(*out);
}
