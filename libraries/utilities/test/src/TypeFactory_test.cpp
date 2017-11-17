////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeFactory_test.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TypeFactory.h"

// testing
#include "testing.h"

//stl
#include <memory>
#include <string>

namespace ell
{
class Base
{
public:
    virtual ~Base() = default;

    static std::string GetTypeName() { return "Base"; }

    virtual std::string GetRuntimeTypeName() const = 0;

    // for the purpose of testing
    virtual void Set() = 0;
    virtual bool Check() = 0;
};

class Derived1 : public Base
{
public:
    static std::string GetTypeName() { return "Derived1"; }

    std::string GetRuntimeTypeName() const override { return GetTypeName(); };

    // for the purpose of testing
    void Set() override
    {
        x = 5;
        y = -17.3;
        s = "test";
    }

    bool Check() override
    {
        if (x == 5 && y == -17.3 && s == "test") return true;
        return false;
    }

private:
    int x = 0;
    double y = 0;
    std::string s;
};

class Derived2 : public Base
{
public:
    static std::string GetTypeName() { return "Derived2"; }

    std::string GetRuntimeTypeName() const override { return GetTypeName(); };

    // for the purpose of testing
    void Set() override
    {
        v.push_back(7.0f);
        v.push_back(8.0f);
    }

    bool Check() override
    {
        if (v.size() != 2) return false;
        if (v[0] == 7.0f && v[1] == 8.0f) return true;
        return false;
    }

private:
    std::vector<float> v;
};

void TypeFactoryTest()
{
    utilities::TypeFactory<Base> factory;
    factory.AddType<Derived1>();
    factory.AddType<Derived2>();

    auto derived1 = factory.Construct(Derived1::GetTypeName());
    auto derived2 = factory.Construct(Derived2::GetTypeName());

    testing::ProcessTest("TypeFactory", derived1->GetRuntimeTypeName() == Derived1::GetTypeName() && derived2->GetRuntimeTypeName() == Derived2::GetTypeName());
}
}
