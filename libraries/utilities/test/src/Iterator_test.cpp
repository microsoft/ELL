////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Iterator_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Iterator.h"
#include "StlIterator.h"
#include "Iterator_test.h"

// testing
#include "testing.h"

#include <vector>
#include <memory>

void TestIterator()
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };
    auto it = utilities::MakeStlIterator(vec.cbegin(), vec.cend());
    auto it2 = utilities::MakeIterator(it);

    bool passed = true;
    int index = 0;
    while(it2.IsValid())
    {
        passed = passed && (it2.Get() == vec[index]);
        it2.Next();
        index++;
    }
            
    testing::ProcessTest("utilities::Iterator.Get", passed);
    testing::ProcessTest("utilities::Iterator length", index == vec.size());
}
