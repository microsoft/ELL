////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     Iterator_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AnyIterator.h"
#include "StlIterator.h"
#include "SupervisedExample.h"
#include "SequentialLineIterator.h"
#include "MappedParser.h"
#include "SparseEntryParser.h"
#include "ParsingIterator.h"

#include "AnyIterator_test.h"

// testing
#include "testing.h"

// stl
#include <vector>

void TestAnyIterator1()
{
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };
    auto it = utilities::MakeStlIterator(vec.cbegin(), vec.cend());
    auto it2 = utilities::MakeAnyIterator(it);

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

void TestAnyIterator2()
{
    std::vector<dataset::SupervisedExample> vec;
    auto it = utilities::MakeStlIterator(vec.cbegin(), vec.cend());
    auto it2 = utilities::MakeAnyIterator(it);

    bool passed = true;
    int index = 0;
    while(it2.IsValid())
    {
        it2.Next();
        index++;
    }
            
    testing::ProcessTest("utilities::Iterator.Get", passed);
    testing::ProcessTest("utilities::Iterator length", index == vec.size());
}

template<typename RowIteratorType, typename VectorEntryParserType>
dataset::ParsingIterator<RowIteratorType, VectorEntryParserType> MakeParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser)
{
    return dataset::ParsingIterator<RowIteratorType, VectorEntryParserType>(std::move(row_iter), std::move(parser));
}

void TestAnyIterator3(bool runTest)
{
    if (!runTest)
        return;

//        Map(const Model& model, const CoordinateList& outputCoordinateList);
    layers::Model model;
    layers::CoordinateList coordinates;
    layers::Map map(model, coordinates);
    
    // create mapped parser for sparse vectors (SVMLight format)
    dataset::MappedParser<dataset::SparseEntryParser> mappedParser(dataset::SparseEntryParser(), map);

    // create line iterator - read line by line sequentially
    dataset::SequentialLineIterator lineIterator("filename");

    // Create iterator
    auto parsingIterator = MakeParsingIterator(std::move(lineIterator), mappedParser);
    auto it = utilities::MakeAnyIterator(parsingIterator);
}
