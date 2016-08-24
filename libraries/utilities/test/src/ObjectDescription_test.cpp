
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescription_test.h"

// utilities
#include "ObjectDescription.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <vector>
#include <sstream>


class TestObject: public utilities::IDescribable
{
public:
    TestObject(int a, double b) : _a(a), _b(b) {}
    TestObject(const utilities::ObjectDescription& description)
    {
        _a = description.GetPropertyValue<int>("a");
        _b = description.GetPropertyValue<double>("b");
    }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        utilities::ObjectDescription description;
        description.AddProperty("a", "Parameter a", _a);
        description.AddProperty("b", "Parameter b", _b);
        return description;
    }

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description;
        description.AddProperty<int>("a", "Parameter a");
        description.AddProperty<double>("b", "Parameter b");
        return description;
    }

private:
    int _a;
    double _b;
};

void TestGetObjectDescription()
{
    TestObject obj(3, 4.5);
    auto desc = obj.GetDescription();

    std::cout << "TestObject has property a: " << desc.HasProperty("a") << std::endl;
    std::cout << "TestObject has property b: " << desc.HasProperty("b") << std::endl;
    std::cout << "TestObject has property c: " << desc.HasProperty("c") << std::endl;
    std::cout << "TestObject.a = " << desc.GetPropertyValue<int>("a") << std::endl;
    std::cout << "TestObject.b = " << desc.GetPropertyValue<double>("b") << std::endl;
}
