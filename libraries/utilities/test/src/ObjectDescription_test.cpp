
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
#include "ObjectDescriptionSerializer.h"

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

    virtual void AddProperties(utilities::Archiver& description) const override
    {
        description.SetType(*this);
        description["a"] = _a;
        description["b"] = _b;
    }

    virtual void SetObjectState(const utilities::Archiver& description, utilities::SerializationContext& context) override
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

    virtual void AddProperties(utilities::Archiver& description) const override
    {
        InnerObject::AddProperties(description);
        description.SetType(*this);
        description["c"] = _c;
    }

    virtual void SetObjectState(const utilities::Archiver& description, utilities::SerializationContext& context) override
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

    virtual void AddProperties(utilities::Archiver& description) const override
    {
        description.SetType(*this);
        description["name"] = _name;
        description["obj"] = _inner;
    }

    virtual void SetObjectState(const utilities::Archiver& description, utilities::SerializationContext& context) override
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
    std::cout << indent << "Type: " << description.GetObjectTypeName(); 
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
    InnerObject innerObj;
    auto innerDescription = innerObj.GetDescription();
//    PrintDescription(innerDescription);

    OuterObject outerObj;
    auto outerDescription = outerObj.GetDescription();
//    PrintDescription(outerDescription);

    DerivedObject derivedObj;
    auto derivedDescription = derivedObj.GetDescription();
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

void TestObjectDescriptionSerializer()
{
    utilities::SerializationContext context;

    utilities::ObjectDescriptionSerializer serializer1;
    utilities::ObjectDescriptionSerializer serializer2;
    utilities::ObjectDescriptionSerializer serializer3;
    
    InnerObject innerObj(3, 4.5);
    serializer1.Serialize("inner", innerObj);

    OuterObject outerObj("Outer", 5, 6.5);
    serializer2.Serialize("outer", outerObj);

    DerivedObject derivedObj(8, 9.5, "derived");
    serializer3.Serialize("derived", derivedObj);

    auto objectDescription1 = serializer1.GetObjectDescription();
    auto objectDescription2 = serializer2.GetObjectDescription();
    auto objectDescription3 = serializer3.GetObjectDescription();


    utilities::ObjectDescriptionDeserializer deserializer1(objectDescription1, context);
    InnerObject deserializedInner;
    deserializer1.Deserialize("inner", deserializedInner);
    testing::ProcessTest("Deserialize with ObjectDescriptionSerializer check",  deserializedInner.GetA() == 3 && deserializedInner.GetB() == 4.5f);        

    utilities::ObjectDescriptionDeserializer deserializer2(objectDescription2, context);
    OuterObject deserializedOuter;
    deserializer2.Deserialize("outer", deserializedOuter);
    testing::ProcessTest("Deserialize with ObjectDescriptionSerializer check",  deserializedOuter.GetName() == "Outer" && deserializedOuter.GetInner().GetA() == 5);        

    utilities::ObjectDescriptionDeserializer deserializer3(objectDescription3, context);
    DerivedObject deserializedDerived;
    deserializer3.Deserialize("derived", deserializedDerived);
    testing::ProcessTest("Deserialize with ObjectDescriptionSerializer check",  deserializedDerived.GetA() == 8 && deserializedDerived.GetB() == 9.5 && deserializedDerived.GetC() == "derived");        
}
