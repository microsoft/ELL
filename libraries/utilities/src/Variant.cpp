////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variant.h"

namespace utilities
{
    //
    // VariantBase implementation
    //
    VariantBase::VariantBase(std::type_index type) : _type(type)
    {};

    //
    // Variant implementation
    //
    Variant::Variant() : _type(std::type_index(typeid(void)))
    {
        _value = nullptr;
    }

    Variant::Variant(const Variant& other) : _type(other._type) 
    {
        if (other._value)
        {
            _value = other._value->Clone();
        }
        else
        {
            _value = nullptr;
        }
    }

    Variant::Variant(std::type_index type, std::unique_ptr<VariantBase> variantValue) : _type(type) 
    { 
        _value = std::move(variantValue); 
    }

    Variant& Variant::operator=(const Variant& other)
    {
        if (other._value != _value)
        {
            _type = other._type;
            _value = other._value->Clone();
        }
        return *this;
    }

    std::string Variant::ToString() const
    {
        return _value->ToString();
    }

    std::string Variant::GetStoredTypeName() const
    {
        return _value->GetStoredTypeName();
    }

    bool Variant::IsEmpty() const
    {
        return _value == nullptr;
    }

    bool Variant::IsPrimitiveType() const
    {
        return _value->IsPrimitiveType();
    }

    bool Variant::IsSerializable() const
    {
        return _value->IsSerializable();
    }

    bool Variant::IsPointer() const
    {
        return _value->IsPointer();
    }

    void Variant::Serialize(const char* name, Serializer& serializer) const
    {
        _value->Serialize(name, serializer);
    }

    void Variant::Deserialize(const char* name, Deserializer& serializer, SerializationContext& context)
    {
//        _value->Deserialize(name, serializer, context);
    }

    std::string to_string(const Variant& variant)
    {
        return variant.ToString();
    }
}
