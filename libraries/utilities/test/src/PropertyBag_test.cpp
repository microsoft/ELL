
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PropertyBag_test.h"

// utilities
#include "JsonArchiver.h"
#include "PropertyBag.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <vector>

namespace ell
{
using namespace utilities;

void TestPropertyBag()
{
    PropertyBag metadata;

    // IsEmpty test
    testing::ProcessTest("PropertyBag::IsEmpty", testing::IsTrue(metadata.IsEmpty()));
    testing::ProcessTest("PropertyBag::HasEntry", testing::IsFalse(metadata.HasEntry("a")));

    metadata.SetEntry("a", std::string("1"));
    testing::ProcessTest("PropertyBag::SetEntry/HasEntry", testing::IsTrue(metadata.HasEntry("a")));
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries", testing::IsEqual(metadata.GetEntry<std::string>("a"), "1"));

    metadata.SetEntry("a", std::string("2"));
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries", testing::IsEqual(metadata.GetEntry<std::string>("a"), "2"));

    auto foo = metadata.GetEntry<std::string>("a");
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries foo", testing::IsEqual(foo, "2"));

    auto removedEntry = metadata.RemoveEntry("a");
    testing::ProcessTest("PropertyBag::RemoveEntry", testing::IsEqual(removedEntry.GetValue<std::string>(), "2"));

    auto removedEntries = metadata.RemoveEntry("a");
    testing::ProcessTest("PropertyBag::IsEmpty", testing::IsTrue(metadata.IsEmpty()));

    metadata.SetEntry("a", 1);
    metadata["a"]++;
    metadata["a"]++;
    metadata["a"]++;
    testing::ProcessTest("PropertyBag::operator[] (non-const)", testing::IsEqual(metadata.GetEntry<int>("a"), 4));

    //
    // Serialization tests
    // 
    metadata.Clear();
    metadata["a"] = std::string("foo");
    metadata["b"] = std::vector<std::string>{ "hello", "world" };
    metadata["c"] = 4;
    metadata["d"] = std::vector<int>{ 5, 6, 7, 8 };
    metadata["e"] = 5.0;

    std::stringstream strstream;
    {
        JsonArchiver archiver(strstream);
        archiver << metadata;
    }

    std::cout << "Archived property bag:" << std::endl;
    std::cout << strstream.str() << std::endl;

    SerializationContext context;
    JsonUnarchiver unarchiver(strstream, context);
    PropertyBag metadata2;
    unarchiver >> metadata2;

    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata2.GetEntry<std::string>("a"), "foo"));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata2.GetEntry<std::vector<std::string>>("b"), { "hello", "world" }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata2.GetEntry<int>("c"), 4));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata2.GetEntry<std::vector<int>>("d"), { 5, 6, 7, 8 }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata2.GetEntry<double>("e"), 5.0));

    SerializationContext context2;
    std::cout << strstream.str() << std::endl;
    std::stringstream strstream2(strstream.str());
    JsonUnarchiver unarchiver2(strstream2, context2);
    PropertyBag metadata3;
    unarchiver2 >> metadata3;
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::string>("a"), "foo"));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::vector<std::string>>("b"), { "hello", "world" }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<int>("c"), 4));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::vector<int>>("d"), { 5, 6, 7, 8 }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<double>("e"), 5.0));
}
} // end namespace
