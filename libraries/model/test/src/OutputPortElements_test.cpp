//
// OutputPortElements tests
//

#include "OutputPortElements_test.h"

// model
#include "ModelGraph.h"
#include "InputNode.h"
#include "OutputPortElements.h"

// testing
#include "testing.h"

// stl
#include <iostream>

void PrintRange(const model::OutputPortRange& range)
{
    std::cout << "[" << range.ReferencedPort()->GetNode()->GetId() << ", " << range.GetStartIndex() << "--" << (range.GetStartIndex()+range.Size()-1) << "]";
}

template <typename T>
void PrintElements(const model::OutputPortElements<T>& elements)
{
    for(const auto& range: elements)
    {
        PrintRange(range);
        std::cout << ", ";
    }
    std::cout << std::endl;
}



void TestSlice()
{
    model::Model g;
    auto in1 = g.AddNode<model::InputNode<double>>(3);
    auto in2 = g.AddNode<model::InputNode<double>>(2);
    auto in3 = g.AddNode<model::InputNode<double>>(4);

    auto allElements = model::OutputPortElements<double>(std::vector<model::OutputPortElements<double>>{model::OutputPortElements<double>{in1->output}, model::OutputPortElements<double>{in2->output}, model::OutputPortElements<double>{in3->output}});
    PrintElements(allElements);
    std::cout << std::endl;

    auto element0 = model::OutputPortElements<double>(allElements, 0);
    PrintElements(element0);

    auto element4 = model::OutputPortElements<double>(allElements, 4);
    PrintElements(element4);

    auto element2_6 = model::OutputPortElements<double>(allElements, 2, 5);
    PrintElements(element2_6);

    // testing::ProcessTest("Testing Size() and iterator count", testing::IsEqual(size1, size2));
}
