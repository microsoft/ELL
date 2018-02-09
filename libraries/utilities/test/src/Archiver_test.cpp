////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Archiver_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archiver_test.h"

// utilities
#include "Archiver.h"
#include "IArchivable.h"
#include "JsonArchiver.h"
#include "UniqueId.h"
#include "XmlArchiver.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <sstream>
#include <vector>

namespace ell
{
    //
    // Types used in tests
    //
struct TestStruct : public utilities::IArchivable
{
    int a = 0;
    float b = 0.0f;
    double c = 0.0;
    TestStruct() = default;
    TestStruct(int a, float b, double c)
        : a(a), b(b), c(c) {}
    static std::string GetTypeName() { return "TestStruct"; }
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    void WriteToArchive(utilities::Archiver& archiver) const override
    {
        archiver["a"] << a;
        archiver["b"] << b;
        archiver["c"] << c;
    }

    void ReadFromArchive(utilities::Unarchiver& archiver) override
    {
        auto objInfo = archiver.GetCurrentObjectInfo();
        testing::ProcessTest("GetCurrentObjectInfo test", objInfo.type == "TestStruct");
        testing::ProcessTest("HasNextPropertyName test", archiver.HasNextPropertyName("a"));
        testing::ProcessTest("HasNextPropertyName test", !archiver.HasNextPropertyName("z"));
        archiver["a"] >> a;
        archiver["b"] >> b;
        testing::ProcessTest("HasNextPropertyName test", archiver.HasNextPropertyName("c"));
        testing::ProcessTest("HasNextPropertyName test", !archiver.HasNextPropertyName("z"));
        archiver["c"] >> c;
    }
};

struct OptionalValueStruct : public utilities::IArchivable
{
    int a = 0;
    int b = 0;
    bool hasB = true;

    OptionalValueStruct() = default;
    OptionalValueStruct(int a)
        : a(a), hasB(false) {}
    OptionalValueStruct(int a, int b)
        : a(a), b(b) {}
    static std::string GetTypeName() { return "OptionalValueStruct"; }
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    void WriteToArchive(utilities::Archiver& archiver) const override
    {
        archiver["a"] << a;
        if (hasB)
        {
            archiver["b"] << b;
        }
    }

    void ReadFromArchive(utilities::Unarchiver& archiver) override
    {
        auto objInfo = archiver.GetCurrentObjectInfo();
        archiver["a"] >> a;
        archiver.OptionalProperty("b", -1) >> b;
    }
};

//
// Test functions
//

void TestArchivedObjectInfo()
{
    using utilities::ArchivedObjectInfo;
    using utilities::ArchiveVersionNumbers;
    
    ArchivedObjectInfo objInfoA1 {"typeA", ArchiveVersionNumbers::v1};
    ArchivedObjectInfo objInfoB1 {"typeB", ArchiveVersionNumbers::v1};
    ArchivedObjectInfo objInfoA2 {"typeA", ArchiveVersionNumbers::v2};
    ArchivedObjectInfo objInfoB2 {"typeB", ArchiveVersionNumbers::v2};
    ArchivedObjectInfo objInfoA2_2 {"typeA", ArchiveVersionNumbers::v2};
    testing::ProcessTest("Testing ArchiveObjectInfo op==", (objInfoA1 == objInfoA1));
    testing::ProcessTest("Testing ArchiveObjectInfo op==", !(objInfoA1 == objInfoB1));
    testing::ProcessTest("Testing ArchiveObjectInfo op==", !(objInfoA1 == objInfoA2));
    testing::ProcessTest("Testing ArchiveObjectInfo op==", !(objInfoA1 == objInfoB2));
    testing::ProcessTest("Testing ArchiveObjectInfo op==", (objInfoA2 == objInfoA2_2));

    testing::ProcessTest("Testing ArchiveObjectInfo op!=", !(objInfoA1 != objInfoA1));
    testing::ProcessTest("Testing ArchiveObjectInfo op!=", (objInfoA1 != objInfoB1));
    testing::ProcessTest("Testing ArchiveObjectInfo op!=", (objInfoA1 != objInfoA2));
    testing::ProcessTest("Testing ArchiveObjectInfo op!=", (objInfoA1 != objInfoB2));
    testing::ProcessTest("Testing ArchiveObjectInfo op!=", !(objInfoA2 != objInfoA2_2));
}

void TestArchiveVersion()
{
    using utilities::ArchiveVersion;
    using utilities::ArchiveVersionNumbers;

    ArchiveVersion v0 = ArchiveVersionNumbers::v0_initial;
    ArchiveVersion v0_2 = ArchiveVersionNumbers::v0_initial;
    ArchiveVersion v1 = ArchiveVersionNumbers::v1;
    ArchiveVersion v2 = ArchiveVersionNumbers::v2;
    ArchiveVersion v3 = ArchiveVersionNumbers::v3_model_metadata;
    testing::ProcessTest("Testing ArchiveVersion comparisons", v0 == v0_2);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v0 <= v0_2);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v0 >= v0_2);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v0 < v1);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v1 <= v2);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v1 < v3);
    testing::ProcessTest("Testing ArchiveVersion comparisons", !(v0 > v1));
    testing::ProcessTest("Testing ArchiveVersion comparisons", !(v1 >= v2));
    testing::ProcessTest("Testing ArchiveVersion comparisons", v3 > v1);
    testing::ProcessTest("Testing ArchiveVersion comparisons", v3 >= v1);
}

template <typename ArchiverType>
void TestArchiver()
{
    bool boolVal = true;
    int intVal = 1;
    float floatVal = 2.5;
    double doubleVal = 3.14;
    TestStruct testStruct{ 1, 2.2f, 3.3 };

    utilities::UniqueId id;

    std::stringstream strstream;
    ArchiverType archiver(strstream);
    archiver.Archive(boolVal);

    archiver.Archive(intVal);

    archiver.Archive(floatVal);

    archiver.Archive(doubleVal);

    archiver.Archive(testStruct);

    archiver.Archive(id);

    // simple stuff
    archiver.Archive(5);

    archiver.Archive(3.1415);

    std::vector<int> intArray{ 1, 2, 3 };
    archiver.Archive("intArray", intArray);

    std::vector<bool> boolArray{ true, false, true };
    archiver.Archive("boolArray", boolArray);

    std::vector<TestStruct> structArray;
    structArray.emplace_back(1, 2.0f, 3.0);
    structArray.emplace_back(4, 5.0f, 6.0);
    structArray.emplace_back(7, 8.0f, 9.0);
    archiver.Archive("structArray", structArray);
}

template <typename ArchiverType, typename UnarchiverType>
void TestUnarchiver()
{
    utilities::SerializationContext context;

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("true", true);
        }

        UnarchiverType unarchiver(strstream, context);
        bool val = false;
        unarchiver.Unarchive("true", val);
        testing::ProcessTest("Deserialize bool check", val == true);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("pi", 3.14159);
        }

        UnarchiverType unarchiver(strstream, context);
        double val = 0;
        unarchiver.Unarchive("pi", val);
        testing::ProcessTest("Deserialize float check", val == 3.14159);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("pie", std::string{ "cherry pie" });
        }

        UnarchiverType unarchiver(strstream, context);
        std::string val;
        unarchiver.Unarchive("pie", val);
        testing::ProcessTest("Deserialize string check", val == "cherry pie");
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            std::vector<int> arr{ 1, 2, 3 };
            archiver.Archive("arr", arr);
        }

        UnarchiverType unarchiver(strstream, context);
        std::vector<int> val;
        unarchiver.Unarchive("arr", val);
        testing::ProcessTest("Deserialize vector<int> check", val[0] == 1 && val[1] == 2 && val[2] == 3);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            TestStruct testStruct{ 1, 2.2f, 3.3 };
            archiver.Archive("s", testStruct);
        }

        UnarchiverType unarchiver(strstream, context);
        TestStruct val;
        unarchiver.Unarchive("s", val);
        testing::ProcessTest("Deserialize IArchivable check", val.a == 1 && val.b == 2.2f && val.c == 3.3);
    }

    {
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            OptionalValueStruct testStruct(1);
            archiver.Archive("s", testStruct);
        }

        UnarchiverType unarchiver(strstream, context);
        OptionalValueStruct val;
        unarchiver.Unarchive("s", val);
        testing::ProcessTest("Deserialize IArchivable with optional values check", val.a == 1 && val.b == -1); // -1 is default value
    }

    {
        // arrays of stuff
        std::stringstream strstream;
        auto doubleVector = std::vector<double>{ 1.0, 2.0, 3.0 };
        std::vector<TestStruct> structVector;
        structVector.push_back(TestStruct{ 1, 2.2f, 3.3 });
        structVector.push_back(TestStruct{ 4, 5.5f, 6.6 });

        {
            ArchiverType archiver(strstream);
            archiver.Archive("vec1", doubleVector);
            archiver.Archive("vec2", structVector);
        }

        UnarchiverType unarchiver(strstream, context);
        std::vector<double> newDoubleVector;
        std::vector<TestStruct> newStructVector;
        unarchiver.Unarchive("vec1", newDoubleVector);
        unarchiver.Unarchive("vec2", newStructVector);

        testing::ProcessTest("Deserialize array check", testing::IsEqual(doubleVector, newDoubleVector));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].a, newStructVector[0].a));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].b, newStructVector[0].b));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[0].c, newStructVector[0].c));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].a, newStructVector[1].a));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].b, newStructVector[1].b));
        testing::ProcessTest("Deserialize array check", testing::IsEqual(structVector[1].c, newStructVector[1].c));
    }

    {
        auto stringVal = std::string{ "Hi there! Here's a tab character: \t, as well as some 'quoted' text." };
        std::stringstream strstream;
        {
            ArchiverType archiver(strstream);
            archiver.Archive("str", stringVal);
        }

        UnarchiverType unarchiver(strstream, context);
        std::string val;
        unarchiver.Unarchive("str", val);
        testing::ProcessTest("Deserialize string check", val == stringVal);
    }
}

void TestJsonArchiver()
{
    TestArchiver<utilities::JsonArchiver>();
}

void TestJsonUnarchiver()
{
    TestUnarchiver<utilities::JsonArchiver, utilities::JsonUnarchiver>();
}

void TestXmlArchiver()
{
    TestArchiver<utilities::XmlArchiver>();
}

void TestXmlUnarchiver()
{
    TestUnarchiver<utilities::XmlArchiver, utilities::XmlUnarchiver>();
}
}
