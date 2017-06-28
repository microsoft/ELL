////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortElements_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortElements_test.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "PortElements.h"

// testing
#include "testing.h"

// utilities
#include "JsonArchiver.h"

// stl
#include <iostream>
#include <sstream>

using namespace ell;

//
// Helpers
//
void PrintRange(const model::PortRange& range)
{
    std::cout << "[" << range.ReferencedPort()->GetNode()->GetId() << ", " << range.GetStartIndex() << "--" << (range.GetStartIndex() + range.Size() - 1) << "]";
}

template <typename T>
void PrintElements(const model::PortElements<T>& elements)
{
    for (const auto& range : elements)
    {
        PrintRange(range);
        std::cout << ", ";
    }
    std::cout << std::endl;
}

//
// Tests
//
void TestSlice()
{
    model::Model g;
    auto in1 = g.AddNode<model::InputNode<double>>(3);
    auto in2 = g.AddNode<model::InputNode<double>>(2);
    auto in3 = g.AddNode<model::InputNode<double>>(4);

    auto allElements = model::PortElements<double>(std::vector<model::PortElements<double>>{ model::PortElements<double>{ in1->output }, model::PortElements<double>{ in2->output }, model::PortElements<double>{ in3->output } });
    auto element0 = model::PortElements<double>(allElements, 0);
    auto element4 = model::PortElements<double>(allElements, 4);
    auto element2_6 = model::PortElements<double>(allElements, 2, 5);
    model::PortElements<double> element2_6_prime = { allElements, 2, 5 };

    testing::ProcessTest("Testing slice and append", testing::IsEqual(allElements.Size(), (size_t)9));
    testing::ProcessTest("Testing slice and append", testing::IsEqual(element0.Size(), (size_t)1));
    testing::ProcessTest("Testing slice and append", testing::IsEqual(element4.Size(), (size_t)1));
}

void TestAppend()
{
    model::Model g;
    auto in1 = g.AddNode<model::InputNode<double>>(3);
    auto in2 = g.AddNode<model::InputNode<double>>(2);
    auto in3 = g.AddNode<model::InputNode<double>>(4);

    model::PortElements<double> elements1 = { in1->output };
    elements1.Append({ in2->output });

    model::PortElements<double> elements2;
    elements2.Append({ in3->output, 1, 2 });
    elements2.Append({ elements1 });

    testing::ProcessTest("Testing Append", testing::IsEqual(elements1.Size(), (size_t)5));
    testing::ProcessTest("Testing Append", testing::IsEqual(elements2.Size(), (size_t)7));
}

void TestParsePortElements()
{
    auto elements = model::ParsePortElementsProxy("123.bar");
    testing::ProcessTest("Testing PortElementProxy::Parse", elements.GetRanges().size() == 1);

    elements = model::ParsePortElementsProxy("123.bar[3:5]");
    testing::ProcessTest("Testing PortElementProxy::Parse", elements.GetRanges()[0].Size() == 2);
}
