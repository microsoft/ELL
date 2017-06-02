////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Parser_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Parser_test.h"

// data
#include "GeneralizedSparseParsingIterator.h"
#include "TextLine.h"
#include "SequentialLineIterator.h"
#include "SingleLineParsingExampleIterator.h"
#include "WeightLabel.h"
#include "AutoDataVector.h"
#include "Dataset.h"

// testing
#include "testing.h"

// stl
#include <string>
#include <sstream>
#include <memory>

namespace ell
{

    std::vector<double> ToArray(data::GeneralizedSparseParsingIterator iterator)
    {
        std::vector<double> vector;
        while (iterator.IsValid())
        {
            auto indexValue = iterator.Get();
            vector.resize(indexValue.index + 1);
            vector[indexValue.index] = indexValue.value;
            iterator.Next();
        }
        return vector;
    }

    void GoodFormatTest(std::string testName, std::string string, std::vector<double> expectedResult)
    {
        auto s = data::TextLine(std::move(string));
        data::GeneralizedSparseParsingIterator p(s);
        auto v = ToArray(p);
        testing::ProcessTest(testName, testing::IsEqual(v, expectedResult));
    }

    void BadFormatTest(std::string testName, std::string string)
    {
        bool exceptionThrown = false;
        try
        {
            auto s = data::TextLine(std::move(string));
            data::GeneralizedSparseParsingIterator p(s);
            auto v = ToArray(p);
        }
        catch (...)
        {
            exceptionThrown = true;
        }
        testing::ProcessTest(testName, exceptionThrown);
    }

    void DataVectorParseTest()
    {
        GoodFormatTest("Good format test 1", "100\t200", { 100, 200 });
        GoodFormatTest("Good format test 2", "      100  \t  \n\t\n     200      \t\n   ", { 100, 200 });
        GoodFormatTest("Good format test 3", "      +100  -200   ", { 100, -200 });
        GoodFormatTest("Good format test 4", "      +100.0  -200.0   ", { 100, -200 });
        GoodFormatTest("Good format test 5", "      +100.5  -200.5   ", { 100.5, -200.5 });
        GoodFormatTest("Good format test 6", "      +.100  -.200   ", { .100, -.200 });
        GoodFormatTest("Good format test 7", "      +1.0e-1  -1.0e+1   ", { .1, -10 });
        GoodFormatTest("Good format test 8", "0:10\t1:20", { 10, 20 });
        GoodFormatTest("Good format test 9", "1:10\t3:20", { 0, 10, 0, 20 });
        GoodFormatTest("Good format test 10", "1:+10.0\t3:-20.00", { 0, 10, 0, -20 });
        GoodFormatTest("Good format test 11", "+1:10\t+1:20", { 10, 20 });
        GoodFormatTest("Good format test 12", "+2:10\t+2:20", { 0, 10, 0, 20 });
        GoodFormatTest("Good format test 13", " +2:10 2:20", { 0, 10, 20 });
        GoodFormatTest("Good format test 14", "100\t200//", { 100, 200 });
        GoodFormatTest("Good format test 15", "100\t200//comment", { 100, 200 });
        GoodFormatTest("Good format test 16", "100\t200// comment 300", { 100, 200 });
        GoodFormatTest("Good format test 17", "100\t200  //comment 300", { 100, 200 });
        GoodFormatTest("Good format test 18", "100\t200#", { 100, 200 });
        GoodFormatTest("Good format test 19", "100\t200#comment 300", { 100, 200 });
        GoodFormatTest("Good format test 20", "100\t200  #  comment 300", { 100, 200 });

        BadFormatTest("Bad format test 1", "1.0:10 2:20");  // double valued index
        BadFormatTest("Bad format test 2", "1X:10 2:20");   // bad character in index
        BadFormatTest("Bad format test 3", "1:10X 2:20");   // bad character in value
        BadFormatTest("Bad format test 4", "+ 1:10 2:20");  // extra whitespace
        BadFormatTest("Bad format test 5", "1 :10 2:20");   // extra whitespace
        BadFormatTest("Bad format test 6", "1: 10 2:20");   // extra whitespace
        BadFormatTest("Bad format test 7", "1:102:20");     // missing whitespace
        BadFormatTest("Bad format test 8", "2:10 2:20");    // nonincreasing index with absolute sparse
        BadFormatTest("Bad format test 9", "10 +0:20");     // nonincreasing index with relative sparse
        BadFormatTest("Bad format test 10", "10 10 1:20");  // nonincreasing index with dense
        BadFormatTest("Bad format test 11", "10 10 1:20  /comment");  // bad comment
        BadFormatTest("Bad format test 11", "10 10 1:20  /");  // bad comment
    }

    void AutoDataVectorParseTest()
    {
        auto parser = data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator>();

        data::TextLine line1("1 2 3 4 5");
        auto dataVector1 = parser.Parse(line1);
        testing::ProcessTest("AutoDataVectorParser test", testing::IsEqual(dataVector1.ToArray(), { 1, 2, 3, 4, 5 })
            && dataVector1.GetInternalType() == data::IDataVector::Type::ByteDataVector);

        data::TextLine line2("0:1 10:5");
        auto dataVector2 = parser.Parse(line2);
        testing::ProcessTest("AutoDataVectorParser test", testing::IsEqual(dataVector2.ToArray(), { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 })
            && dataVector2.GetInternalType() == data::IDataVector::Type::SparseByteDataVector);
    }

    void SingleFileParseTest()
    {
        auto string = R"aw(

    // comment

    1.0     0:1 1:2 2:3# comment


    # comment

    -1.0    3:3 10:3 // this is the second row
    1.0     2.7 4:-.3 10:3.14

    // another comment

    )aw";

        std::stringstream stream(string);
        data::SequentialLineIterator textLineIterator(stream);
        data::LabelParser metadataParser;
        data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator> dataVectorParser;

        auto exampleIterator = data::MakeSingleLineParsingExampleIterator(std::move(textLineIterator), std::move(metadataParser), std::move(dataVectorParser));
        auto dataset = data::MakeDataset(std::move(exampleIterator));

        testing::ProcessTest("SingleFileParse test1", dataset[0].GetMetadata().label == 1 && testing::IsEqual(dataset[0].GetDataVector().ToArray(), { 1, 2, 3 }));
        testing::ProcessTest("SingleFileParse test2", dataset[1].GetMetadata().label == -1 && testing::IsEqual(dataset[1].GetDataVector().ToArray(), { 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3 }));
        testing::ProcessTest("SingleFileParse test3", dataset[2].GetMetadata().label == 1 && testing::IsEqual(dataset[2].GetDataVector().ToArray(), { 2.7, 0, 0, 0, -0.3, 0, 0, 0, 0, 0, 3.14 }));
    }
}