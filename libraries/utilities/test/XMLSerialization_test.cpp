////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization_test.cpp (XMLSerialization_test)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSerialization.h"

// testing
#include "testing.h"

//stl
#include <string>
#include <memory>

class Base
{
public:
    static const char* GetTypeName() { return "Base"; }
    
    virtual const char* GetRuntimeTypeName() const = 0;
    
    virtual void Read(utilities::XMLDeserializer& deserializer) = 0;
    
    virtual void Write(utilities::XMLSerializer& serializer) const = 0;

    // for the purpose of testing
    virtual void Set() = 0;
    virtual bool Check() = 0;
};

class Derived1 : public Base
{
    static const char* GetTypeName() { return "Derived1"; }

    virtual const char* GetRuntimeTypeName() const override { return GetTypeName(); };

    virtual void Read(utilities::XMLDeserializer& deserializer) override
    {
        deserializer.Deserialize("x", x);
        deserializer.Deserialize("y", y);
        deserializer.Deserialize("s", s);
    }

    virtual void Write(utilities::XMLSerializer& serializer) const override
    {
        serializer.Serialize("x", x);
        serializer.Serialize("y", y);
        serializer.Serialize("s", s);
    }

    // for the purpose of testing
    virtual void Set() override
    {
        x = 5;
        y = -17.3;
        s = "test";
    }

    virtual bool Check() override
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
    static const char* GetTypeName() { return "Derived2"; }

    virtual const char* GetRuntimeTypeName() const override { return GetTypeName(); };

    virtual void Read(utilities::XMLDeserializer& deserializer) override
    {
        deserializer.Deserialize("v", v);
    }

    virtual void Write(utilities::XMLSerializer& serializer) const override
    {
        serializer.Serialize("v", v);
    }

    // for the purpose of testing
    virtual void Set() override
    {
        v.push_back(7.0f);
        v.push_back(8.0f);
    }

    virtual bool Check() override
    {
        if(v.size() != 2) return false;
        if (v[0] == 7.0f && v[1] == 8.0f) return true;
        return false;
    }

private:
    std::vector<float> v;
};

void Read(std::string runtimeTypeName, std::unique_ptr<Base>& value)
{
    if(runtimeTypeName == "Derived1")
    {
        value = std::make_unique<Derived1>();
    }
    else if(runtimeTypeName == "Derived2")
    {
        value = std::make_unique<Derived2>();
    }
    else
    {
        throw std::runtime_error("attempted to deserialize an unrecognized class type");
    }
}

void XMLSerializationTest()
{
    std::vector<std::unique_ptr<Base>> vec;
    vec.push_back(std::make_unique<Derived1>());
    vec.push_back(std::make_unique<Derived2>());
    vec[0]->Set();
    vec[1]->Set();

    std::stringstream ss;
    utilities::XMLSerializer serializer(ss);
    serializer.Serialize("vec", vec);

    std::cout << ss.str() << std::endl;

    utilities::XMLDeserializer deserializer(ss);
    std::vector<std::unique_ptr<Base>> vec2;
    deserializer.Deserialize("vec", vec2);

    testing::ProcessTest("utilities::XMLSerialization", vec2[0]->Check() && vec[1]->Check());
}

int main()
{
    XMLSerializationTest();

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}