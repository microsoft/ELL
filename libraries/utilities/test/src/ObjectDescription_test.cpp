
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

class ChildObject : public utilities::IDescribable
{
public:
    ChildObject(int a, double b) : _a(a), _b(b) {}
    ChildObject(const utilities::ObjectDescription& description)
    {
        _a = description.GetPropertyValue<int>("a");
        _b = description.GetPropertyValue<double>("b");
    }

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description("Child object");
        description.AddProperty<int>("a", "Parameter a");
        description.AddProperty<double>("b", "Parameter b");
        return description;
    }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        utilities::ObjectDescription description = GetTypeDescription();
        description["a"] = _a;
        description["b"] = _b;
        return description;
    }

    static std::string GetTypeName() { return "ChildObject"; }
    virtual std::string GetRuntimeTypeName() { return GetTypeName(); }

private:
    int _a;
    double _b;
};

class ParentObject : public utilities::IDescribable
{
public:
    ParentObject(std::string name, int a, double b) : _name(name), _child(a, b) {}
    //ParentObject(const utilities::ObjectDescription& description)
    //{
    //}

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description("Parent object");
        description.AddProperty<decltype(_name)>("name", "Name");
        description.AddProperty<decltype(_child)>("child", "Child object");
        return description;
    }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        utilities::ObjectDescription description = GetTypeDescription();
        description["name"] = _name;
        description["child"] = &_child;
        return description;
    }

    static std::string GetTypeName() { return "ParentObject"; }
    virtual std::string GetRuntimeTypeName() { return GetTypeName(); }


private:
    std::string _name;
    ChildObject _child;
};

void TestGetObjectDescription()
{
    ChildObject childObj(3, 4.5);
    auto childDescription = childObj.GetDescription();

    std::cout << "ChildObject description: " << childDescription.GetDescription() << std::endl;
    std::cout << "ChildObject properties" << std::endl;
    for(const auto& iter: childDescription.Properties())
    {
        auto name = iter.first;
        auto prop = iter.second;
        std::cout << name << " -- " << prop.GetPropertyTypeName() << ": " << prop.GetDescription() << std::endl;
    }

    ParentObject parentObj("Parent", 5, 6.5);
    auto parentDescription = parentObj.GetDescription();
    std::cout << "ParentObject description: " << parentDescription.GetDescription() << std::endl;
    std::cout << "ParentObject properties" << std::endl;
    for(const auto& iter: parentDescription.Properties())
    {
        auto name = iter.first;
        auto prop = iter.second;
        std::cout << name << " -- " << prop.GetPropertyTypeName() << ": " << prop.GetDescription() << std::endl;
    }

    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !childDescription.HasProperty("c"));
    testing::ProcessTest("ObjectDescription", childDescription.GetPropertyValue<int>("a") == 3);
    testing::ProcessTest("ObjectDescription", childDescription.GetPropertyValue<double>("b") == 4.5);
}
