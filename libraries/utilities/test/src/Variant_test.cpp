
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variant_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variant_test.h"

// utilities
#include "TypeName.h"
#include "Variant.h"
#include "JsonArchiver.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <vector>

namespace ell
{
//
// Debug helpers
//
template <typename T>
void PrintVector(const std::vector<T>& v)
{
    utilities::JsonArchiver ar(std::cout);
    ar << v;
    std::cout << std::endl;
}

//
// Types
//
struct TestStruct
{
    int a;
    float b;
    static std::string GetTypeName() { return "TestStruct"; }
};

enum Enum1
{
    a = 0,
    b,
    c
};

enum class Enum2
{
    A = 0,
    B,
    C
};

//
// Tests
//
void TestScalarVariant()
{
    utilities::Variant v = utilities::MakeVariant<int>(5);

    testing::ProcessTest("Variant check type", v.IsType<int>());
    testing::ProcessTest("Variant check type", !v.IsType<float>());
    testing::ProcessTest("Variant check type", !v.IsType<bool>());

    testing::ProcessTest("Variant check value", v.GetValue<int>() == 5);

    v = (int)3;
    testing::ProcessTest("Variant check type", v.IsType<int>());

    v = (float)3.25;
    testing::ProcessTest("Variant check type", v.IsType<float>());
    testing::ProcessTest("Variant check value", v.GetValue<float>() == 3.25);

    bool success;
    float xFloat = 0;
    success = v.TryGetValue(xFloat);
    testing::ProcessTest("Variant TryGetValue on correct type", success);
    testing::ProcessTest("Variant TryGetValue on correct type", xFloat == 3.25f);

    double xDouble;
    success = v.TryGetValue(xDouble);
    testing::ProcessTest("Variant TryGetValue on wrong value", !success);
}

void TestVectorVariant()
{
    auto intVecName = utilities::TypeName<std::vector<int>>::GetName();
    utilities::Variant v = utilities::MakeVariant<std::vector<int>>(std::vector<int>{ 3 });

    testing::ProcessTest("Variant check type", v.IsType<std::vector<int>>());
    testing::ProcessTest("Variant check type", !v.IsType<float>());
    testing::ProcessTest("Variant check type", !v.IsType<bool>());

    testing::ProcessTest("Variant check value", v.GetValue<std::vector<int>>()[0] == 3);
}

void TestObjectVariant()
{
    TestStruct a;
    utilities::Variant v = utilities::Variant(a);
}

void TestVariantGetValueAs()
{
    // Exercise GetValueAs and TryGetValueAs
    utilities::Variant v1(3);
    utilities::Variant v2(size_t(4));
    utilities::Variant v3(3.4);
    utilities::Variant v4(3.2f);
    utilities::Variant v5(std::string("hello"));

    testing::ProcessTest("Variant GetValueAs", v1.GetValueAs<short>() == 3);
    testing::ProcessTest("Variant GetValueAs", v2.GetValueAs<short>() == 4);
    testing::ProcessTest("Variant GetValueAs", v3.GetValueAs<short>() == 3);
    testing::ProcessTest("Variant GetValueAs", v4.GetValueAs<double>() == static_cast<double>(3.2f));
    testing::ProcessTest("Variant GetValueAs", v5.GetValueAs<std::string>() == "hello");
}

void TestVariantParseSimple()
{
    auto v1 = utilities::MakeVariant<int>();
    v1.ParseInto("3");

    auto v2 = utilities::MakeVariant<std::string>();
    v2.ParseInto("hello");

    auto v3 = utilities::MakeVariant<Enum1>();
    v3.ParseInto("1");

    auto v4 = utilities::MakeVariant<Enum2>();
    v4.ParseInto("1");

    testing::ProcessTest("Variant ParseInto", v1.GetValue<int>() == 3);
    testing::ProcessTest("Variant ParseInto", v2.GetValue<std::string>() == "hello");
    testing::ProcessTest("Variant ParseInto", v3.GetValue<Enum1>() == b);
    testing::ProcessTest("Variant ParseInto", v4.GetValue<Enum2>() == Enum2::B);

    // Now test failures

    bool success1 = v1.TryParseInto("hello");
    bool success2 = v2.TryParseInto("hello");
    bool success3 = v3.TryParseInto("hello");
    bool success4 = v4.TryParseInto("hello");
    testing::ProcessTest("Variant TryParseInto", !success1); // should fail
    testing::ProcessTest("Variant TryParseInto", success2); // should succeed
    testing::ProcessTest("Variant TryParseInto", !success3); // should fail
    testing::ProcessTest("Variant TryParseInto", !success4); // should fail
}

void TestParseVectorVaraint()
{
    auto v = utilities::MakeVariant<std::vector<int>>();
    bool success = v.TryParseInto("[1,2,3]");
    testing::ProcessTest("Variant ParseInto vector", success);
    if (!success) return;

    PrintVector(v.GetValue<std::vector<int>>());

    testing::ProcessTest("Variant ParseInto vector", v.GetValue<std::vector<int>>() == std::vector<int>{ 1, 2, 3 });
}

void TestVariantToString()
{
    // Exercise ToString or op<<
    utilities::Variant v1(5);
    utilities::Variant v2(std::string("hello"));
    utilities::Variant v3(3.0);
    utilities::Variant v4(Enum1::a);

    testing::ProcessTest("Variant ToString", v1.ToString() == "5");
    testing::ProcessTest("Variant ToString", v2.ToString() == "hello");
    testing::ProcessTest("Variant ToString", v3.ToString().substr(0, 3) == "3.0");
    testing::ProcessTest("Variant ToString", v4.ToString() == "0");
}

void TestVariantArchive()
{
    // Serialization
    utilities::Variant variant(std::string("hello"));

    std::stringstream strstream;
    {
        utilities::JsonArchiver archiver(strstream);
        // archiver << variant;
    }

    std::cout << "Archived variant:" << std::endl;
    std::cout << strstream.str() << std::endl;

    utilities::SerializationContext context;
    utilities::JsonUnarchiver unarchiver(strstream, context);
    utilities::Variant variant2;
    // unarchiver >> variant2;
}

} // end namespace
