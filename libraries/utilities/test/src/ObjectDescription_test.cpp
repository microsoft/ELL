
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
#include "ObjectDescriptionArchiver.h"

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

    virtual void Serialize(utilities::Archiver& archiver) const override
    {
        archiver["a"] << _a;
        archiver["b"] << _b;
    }

    virtual void Deserialize(utilities::Unarchiver& archiver) override
    {
        archiver["a"] >> _a;
        archiver["b"] >> _b;
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

    virtual void Serialize(utilities::Archiver& archiver) const override
    {
        InnerObject::Serialize(archiver);
        archiver["c"] << _c;
    }

    virtual void Deserialize(utilities::Unarchiver& archiver) override
    {
        InnerObject::Deserialize(archiver);
        archiver["c"] >> _c;
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

    virtual void Serialize(utilities::Archiver& archiver) const override
    {
        archiver["name"] << _name;
        archiver["obj"] << _inner;
    }

    virtual void Deserialize(utilities::Unarchiver& archiver) override
    {
        archiver["name"] >> _name;
        archiver["obj"] >> _inner;
    }

    static std::string GetTypeName() { return "OuterObject"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    std::string _name;
    InnerObject _inner;
};

void PrintDescription(const utilities::ObjectDescription& description, std::string name="", size_t indentCount = 0)
{
    std::string indent(4*indentCount, ' ');
    std::cout << indent << name << " Type: " << description.GetObjectTypeName(); 
    if(description.HasValue())
    {
        std::cout << " = " << description.GetValueString();
    }
    std::cout << std::endl;    

    for(const auto& iter: description.GetProperties())
    {
        auto name = iter.first;
        auto prop = iter.second;
        PrintDescription(prop, name, indentCount+1);
    }
}

void TestGetTypeDescription()
{
    InnerObject innerObj;
    auto innerDescription = innerObj.GetDescription();
    PrintDescription(innerDescription);

    OuterObject outerObj;
    auto outerDescription = outerObj.GetDescription();
    PrintDescription(outerDescription);

    DerivedObject derivedObj;
    auto derivedDescription = derivedObj.GetDescription();
    PrintDescription(derivedDescription);

    testing::ProcessTest("GetDescription", innerDescription.HasProperty("a"));
    testing::ProcessTest("GetDescription", innerDescription.HasProperty("b"));
    testing::ProcessTest("GetDescription", !innerDescription.HasProperty("c"));

    testing::ProcessTest("GetDescription", outerDescription.HasProperty("name"));
    testing::ProcessTest("GetDescription", outerDescription.HasProperty("obj"));

    testing::ProcessTest("GetDescription", derivedDescription.HasProperty("a"));
    testing::ProcessTest("GetDescription", derivedDescription.HasProperty("b"));
    testing::ProcessTest("GetDescription", derivedDescription.HasProperty("c"));
}

void TestGetObjectDescription()
{
    InnerObject innerObj(3, 4.5);
    auto innerDescription = innerObj.GetDescription();
    PrintDescription(innerDescription);
    std::cout << std::endl;

    OuterObject outerObj("Outer", 5, 6.5);
    auto outerDescription = outerObj.GetDescription();
    PrintDescription(outerDescription);
    std::cout << std::endl;

    DerivedObject derivedObj(8, 9.5, "derived");
    auto derivedDescription = derivedObj.GetDescription();
    PrintDescription(derivedDescription);
    std::cout << std::endl;

    // Inner
    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("a"));
    testing::ProcessTest("ObjectDescription", innerDescription.HasProperty("b"));
    testing::ProcessTest("ObjectDescription", !innerDescription.HasProperty("c"));
    testing::ProcessTest("ObjectDescription", innerDescription["a"].GetValue<int>() == 3);
    testing::ProcessTest("ObjectDescription", innerDescription["b"].GetValue<double>() == 4.5);

    // Outer
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("name"));
    testing::ProcessTest("ObjectDescription", outerDescription.HasProperty("obj"));
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

void TestObjectDescriptionArchiver()
{
    utilities::SerializationContext context;
    utilities::ObjectDescriptionArchiver serializer1;
    utilities::ObjectDescriptionArchiver serializer2;
    utilities::ObjectDescriptionArchiver serializer3;
    
    InnerObject innerObj(3, 4.5);
    serializer1.Serialize(innerObj);

    OuterObject outerObj("Outer", 5, 6.5);
    serializer2.Serialize(outerObj);

    DerivedObject derivedObj(8, 9.5, "derived");
    serializer3.Serialize(derivedObj);

    auto objectDescription1 = serializer1.GetObjectDescription();
    PrintDescription(objectDescription1);
    std::cout << std::endl;

    auto objectDescription2 = serializer2.GetObjectDescription();
    PrintDescription(objectDescription2);
    std::cout << std::endl;

    auto objectDescription3 = serializer3.GetObjectDescription();
    PrintDescription(objectDescription3);
    std::cout << std::endl;

    utilities::ObjectDescriptionUnarchiver deserializer1(objectDescription1, context);
    InnerObject deserializedInner;
    deserializer1.Deserialize(deserializedInner);
    testing::ProcessTest("Deserialize with ObjectDescriptionArchiver check",  deserializedInner.GetA() == 3 && deserializedInner.GetB() == 4.5f);        

    // TODO: fix error with deserializing compound objects
    utilities::ObjectDescriptionUnarchiver deserializer2(objectDescription2, context);
    OuterObject deserializedOuter;
    deserializer2.Deserialize(deserializedOuter);
    testing::ProcessTest("Deserialize with ObjectDescriptionArchiver check",  deserializedOuter.GetName() == "Outer" && deserializedOuter.GetInner().GetA() == 5);        

    utilities::ObjectDescriptionUnarchiver deserializer3(objectDescription3, context);
    DerivedObject deserializedDerived;
    deserializer3.Deserialize(deserializedDerived);
    testing::ProcessTest("Deserialize with ObjectDescriptionArchiver check",  deserializedDerived.GetA() == 8 && deserializedDerived.GetB() == 9.5 && deserializedDerived.GetC() == "derived");        
}
