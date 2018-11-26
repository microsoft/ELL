////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeName_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <utilities/include/TypeName.h>
#include <utilities/include/UniqueId.h>

#include <testing/include/testing.h>

#include <string>

namespace ell
{
template <typename T>
void VerifyTypeName(const std::string& name)
{
    testing::ProcessTest(std::string("TypeName<") + name + ">", utilities::TypeName<T>::GetName() == name);
}

void TestFundamentalTypeNames()
{
    VerifyTypeName<bool>("bool");
    VerifyTypeName<char>("char");
    VerifyTypeName<int>("int");
    VerifyTypeName<float>("float");
    VerifyTypeName<double>("double");
}

void TestClassTypeNames()
{
    VerifyTypeName<utilities::UniqueId>("UniqueId");
}

enum SimpleEnum
{
    A,
    B,
    C
};

enum class ClassEnum
{
    a,
    b,
    c
};

void TestEnumTypeNames()
{
    VerifyTypeName<SimpleEnum>("enum");
    VerifyTypeName<ClassEnum>("enum");
}
} // namespace ell
