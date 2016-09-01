
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
#include "Serializer.h"
#include "XmlSerializer.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <vector>
#include <sstream>

class InnerObject : public utilities::IDescribable
{
public:
    InnerObject() = default;
    InnerObject(int a, double b) : _a(a), _b(b) {}
    int GetA() const { return _a; }
    double GetB() const { return _b; }

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<InnerObject>("Inner object");
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

    virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context) override
    {
        description["a"] >> _a;
        description["b"] >> _b;
    }

    static std::string GetTypeName() { return "InnerObject"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    int _a = 0;
    double _b = 0.0;
};

class DerivedObject : public InnerObject
{
public:
    DerivedObject() = default;
    DerivedObject(int a, double b, std::string c) : InnerObject(a, b), _c(c) {}
    std::string GetC() { return _c; }

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<InnerObject, DerivedObject>("Derived object");
        description.AddProperty<std::string>("c", "Parameter c");
        return description;
    }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        auto description = GetParentDescription<InnerObject, DerivedObject>();
        description["c"] = _c;
        return description;
    }

    virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context) override
    {
        InnerObject::SetObjectState(description, context);
        description["c"] >> _c;
    }

    static std::string GetTypeName() { return "DerivedObject"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    std::string _c = "";
};

class OuterObject : public utilities::IDescribable
{
public:
    OuterObject() = default;
    OuterObject(std::string name, int a, double b) : _name(name), _inner(a, b) {}
    std::string GetName() { return _name; }
    InnerObject GetInner() { return _inner; }

    static utilities::ObjectDescription GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<OuterObject>("Outer object");
        description.AddProperty<decltype(_name)>("name", "Name");
        description.AddProperty<decltype(_inner)>("obj", "Inner object");
        return description;
    }

    virtual utilities::ObjectDescription GetDescription() const override
    {
        utilities::ObjectDescription description = GetTypeDescription();
        description["name"] = _name;
        description["obj"] = _inner;
        return description;
    }

    virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context) override
    {
        description["name"] >> _name;
        description["obj"] >> _inner;
    }

    static std::string GetTypeName() { return "OuterObject"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    std::string _name;
    InnerObject _inner;
};

void PrintDescription(const utilities::ObjectDescription& description, size_t indentCount = 0)
{
    std::string indent(4*indentCount, ' ');
    std::cout << indent << "Type: " << description.GetObjectTypeName() << " -- " << description.GetDocumentation(); 
    if(description.HasValue())
    {
        std::cout << " = " << description.GetValueString();
    }
    std::cout << std::endl;    

    for(const auto& iter: description.GetProperties())
    {
        auto name = iter.first;
        auto prop = iter.second;
        PrintDescription(prop, indentCount+1);
    }
}

void TestGetTypeDescription()
{
    auto innerDescription = InnerObject::GetTypeDescription();
//    PrintDescription(innerDescription);

    auto outerDescription = OuterObject::GetTypeDescription();
//    PrintDescription(outerDescription);

    auto derivedDescription = DerivedObject::GetTypeDescription();
//    PrintDescription(derivedDescription);

    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !innerDescription.HasProperty("c"));

    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("name"));
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("obj"));

    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("c"));
}

void TestGetObjectDescription()
{
    InnerObject innerObj(3, 4.5);
    auto innerDescription = innerObj.GetDescription();
//    PrintDescription(innerDescription);

    OuterObject outerObj("Outer", 5, 6.5);
    auto outerDescription = outerObj.GetDescription();
//    PrintDescription(outerDescription);

    DerivedObject derivedObj(8, 9.5, "derived");
    auto derivedDescription = derivedObj.GetDescription();
//    PrintDescription(derivedDescription);

    // Inner
    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !innerDescription.HasProperty("c"));
    testing::ProcessTest("ObjectDescription", innerDescription["a"].GetValue<int>() == 3);
    testing::ProcessTest("ObjectDescription", innerDescription["b"].GetValue<double>() == 4.5);

    // Outer
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("name"));
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("obj"));
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("name"));
    testing::ProcessTest("ObjectDescription", outerDescription["name"].GetValue<std::string>() == "Outer");
    auto outerInnerDescription = outerDescription["obj"];
    testing::ProcessTest("ObjectDescription", outerInnerDescription["a"].GetValue<int>() == 5);
    testing::ProcessTest("ObjectDescription", outerInnerDescription["b"].GetValue<double>() == 6.5);

    // Derived
    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", derivedDescription.HasProperty("c"));
    testing::ProcessTest("ObjectDescription", derivedDescription["a"].GetValue<int>() == 8);
    testing::ProcessTest("ObjectDescription", derivedDescription["b"].GetValue<double>() == 9.5);
    testing::ProcessTest("ObjectDescription", derivedDescription["c"].GetValue<std::string>() == "derived");
}

void TestSerializeIDescribable()
{
    utilities::SerializationContext context;
    std::stringstream strstream;
    {
        utilities::SimpleXmlSerializer serializer(strstream);
     
        InnerObject innerObj(3, 4.5);
        serializer.Serialize("inner", innerObj);

        OuterObject outerObj("Outer", 5, 6.5);
        serializer.Serialize("outer", outerObj);

        DerivedObject derivedObj(8, 9.5, "derived");
        serializer.Serialize("derived", derivedObj);

        // print
        std::cout << "Serialized stream:" << std::endl;
        std::cout << strstream.str() << std::endl;
    }

    utilities::SimpleXmlDeserializer deserializer(strstream, context);
    InnerObject deserializedInner;

    deserializer.Deserialize("inner", deserializedInner);
    testing::ProcessTest("Deserialize IDescribable check",  deserializedInner.GetA() == 3 && deserializedInner.GetB() == 4.5f);        

    OuterObject deserializedOuter;
    deserializer.Deserialize("outer", deserializedOuter);
    testing::ProcessTest("Deserialize IDescribable check",  deserializedOuter.GetName() == "Outer" && deserializedOuter.GetInner().GetA() == 5);        

    DerivedObject deserializedDerived;
    deserializer.Deserialize("derived", deserializedDerived);
    testing::ProcessTest("Deserialize IDescribable check",  deserializedDerived.GetA() == 8 && deserializedDerived.GetB() == 9.5 && deserializedDerived.GetC() == "derived");        
}