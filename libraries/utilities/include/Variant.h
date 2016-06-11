////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//
// Variant class
//

#include <memory>
#include <typeindex>
#include <stdexcept>

/// <summary> utilities namespace </summary>
namespace utilities
{

// Forward declaration of derived class
template <typename ValueType> class VariantDerived;

/// <summary> Base class for value holder
class VariantBase
{
public:
    VariantBase(std::type_index type) : _type(type) {}
    virtual ~VariantBase() {};

    template <typename ValueType>
    const ValueType& GetValue() const
    {
        if(std::type_index(typeid(ValueType)) != _type)
        {
            throw std::runtime_error("Incorrect type");
        }

        auto thisPtr = dynamic_cast<const VariantDerived<ValueType>*>(this);
        if(thisPtr == nullptr)
        {
            throw std::runtime_error("Bad dynamic cast!");
        }

        return thisPtr->GetValue();
    }

    std::type_index GetType() const 
    {
        return _type;
    }

private:
    std::type_index _type; // redundant
};

/// <summary> 
template <typename ValueType>
class VariantDerived : public VariantBase
{    
public:
    VariantDerived(const ValueType& val) : VariantBase(typeid(ValueType)), _value(val) {}
    const ValueType& GetValue() const { return _value;}

private:
    ValueType _value;
};


/// <summary> A class that can hold any kind of value and provide a type-safe way to access it
class Variant
{
public:
    Variant(std::unique_ptr<VariantBase>&& variantValue) : _type(variantValue->GetType())
    {
        _value = std::move(variantValue);
    }

    // template <typename SimpleValueType> Variant(SimpleValueType value)
    // {
    //     // set the union somehow. Or should we have a tuple?
    // }

    // get value
    template <typename ValueType>
    const ValueType& GetValue() const
    {
        if(std::type_index(typeid(ValueType)) != _type)
        {
            throw std::runtime_error("Bad variant access");
        }

        return _value->GetValue<ValueType>();
    }

    template <typename ValueType>
    bool IsType() const
    {
        return (std::type_index(typeid(ValueType)) == _type);
    }

    // assignment

private:
    std::type_index _type; // crap. cant' change a reference after it's been assigned
    std::unique_ptr<VariantBase> _value;

    // efficiency hack:
    // for often-used simple types, have a union of them and use that instead of a pointer
    // 
    
    /*
    union
    {
        bool boolValue;
        int32_t int32Value;
        int64_t int64Value;
        float floatValue;
        double doubleValue;        
    } _simpleValue;
    */
};

template <typename ValueType, typename... Args>
Variant MakeVariant(Args... args)
{
    ValueType obj(args...);

    auto derivedPtr = new VariantDerived<ValueType>(obj);
    auto basePtr = dynamic_cast<VariantBase*>(derivedPtr);
    return Variant(std::unique_ptr<VariantBase>(basePtr));
}
}

#include "../tcc/Variant.tcc"
