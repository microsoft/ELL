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
    static std::string GetTypeName() { return std::string("Base"); }
    
    virtual std::string GetRuntimeTypeName() const = 0;
    
    virtual void Read(utilities::XMLDeserializer& deserializer) = 0;
    
    virtual void Write(utilities::XMLSerializer& serializer) const = 0;

    // for the purpose of testing
    virtual void Set() = 0;
    virtual bool Check() = 0;
};

class Derived1 : public Base
{
    static std::string GetTypeName() { return std::string("Derived1"); }

    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); };

    virtual void Read(utilities::XMLDeserializer& deserializer) override
    {
        deserializer.Deserialize("x", x);
        deserializer.Deserialize("y", y);
    }

    virtual void Write(utilities::XMLSerializer& serializer) const override
    {
        serializer.Serialize("x", x);
        serializer.Serialize("y", y);
    }

    // for the purpose of testing
    virtual void Set() override
    {
        x = 5;
        y = -17.3;
    }

    virtual bool Check() override
    {
        if (x == 5 && y == -17.3) return true;
        return false;
    }

private:
    int x = 0;
    double y = 0;
};

class Derived2 : public Base
{
    static std::string GetTypeName() { return std::string("Derived2"); }

    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); };

    virtual void Read(utilities::XMLDeserializer& deserializer) override
    {
        deserializer.Deserialize("a", a);
        deserializer.Deserialize("b", b);
    }

    virtual void Write(utilities::XMLSerializer& serializer) const override
    {
        serializer.Serialize("a", a);
        serializer.Serialize("b", b);
    }

    // for the purpose of testing
    virtual void Set() override
    {
        a = 99.9f;
        b = 100000;
    }

    virtual bool Check() override
    {
        if (a == 99.9f && b == 100000) return true;
        return false;
    }

private:
    float a = 0;
    unsigned long b = 0;
};

void Read(std::string derivedTypeName, std::shared_ptr<Base>& spValue)
{
    if(derivedTypeName == "Derived1")
    {
        spValue = std::make_shared<Derived1>();
    }
    else if(derivedTypeName == "Derived2")
    {
        spValue = std::make_shared<Derived2>();
    }
    else
    {
        throw std::runtime_error("attempted to deserialize an unrecognized class type");
    }
}

void XMLSerializationTest()
{
    std::vector<std::shared_ptr<Base>> vec;
    vec.push_back(std::make_shared<Derived1>());
    vec.push_back(std::make_shared<Derived2>());
    vec[0]->Set();
    vec[1]->Set();

    utilities::XMLSerializer serializer;
    serializer.Serialize("vec", vec);

    std::stringstream ss;
    serializer.WriteToStream(ss);
    std::cout << ss.str() << std::endl;

    utilities::XMLDeserializer deserializer(ss);
    std::vector<std::shared_ptr<Base>> vec2;
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