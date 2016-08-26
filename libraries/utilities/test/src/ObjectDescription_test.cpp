
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
        utilities::ObjectDescription description = utilities::ObjectDescription::MakeObjectDescription<ChildObject>("Child object");
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
        utilities::ObjectDescription description = utilities::ObjectDescription::MakeObjectDescription<ParentObject>("Parent object");
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

void PrintDescription(const utilities::ObjectDescription& description, size_t indentCount = 0)
{
    std::string indent(4*indentCount, ' ');
    std::cout << indent << "Type: " << description.GetObjectTypeName() << " description: " << description.GetDescription() << std::endl;    
    indent += "    ";
    for(const auto& iter: description.Properties())
    {
        auto name = iter.first;
        auto prop = iter.second;
        std::cout << indent << name << " -- " << prop.GetObjectTypeName() << ": " << prop.GetDescription() << std::endl;
         
        // TODO: if this property is describable, get its description and print it (recursively)
        if(prop.IsDescribable())
        {
            prop.FillInDescription();
            PrintDescription(prop, indentCount+1);
        }
    }
}

void TestGetTypeDescription()
{
    auto childDescription = ChildObject::GetTypeDescription();
    PrintDescription(childDescription);

    auto parentDescription = ParentObject::GetTypeDescription();
    PrintDescription(parentDescription);

    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !childDescription.HasProperty("c"));
}

void TestGetObjectDescription()
{
    ChildObject childObj(3, 4.5);
    auto childDescription = childObj.GetDescription();
    PrintDescription(childDescription);

    ParentObject parentObj("Parent", 5, 6.5);
    auto parentDescription = parentObj.GetDescription();
    PrintDescription(parentDescription);

    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", childDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !childDescription.HasProperty("c"));
    testing::ProcessTest("ObjectDescription", childDescription.GetPropertyValue<int>("a") == 3);
    testing::ProcessTest("ObjectDescription", childDescription.GetPropertyValue<double>("b") == 4.5);
}
