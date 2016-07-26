////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescription.h"
#include "Variant.h"

namespace utilities
{
    //
    // Variant
    //

    Variant::Variant(const Variant& other) : _type(other._type) { _value = other._value->Clone(); }

    Variant::Variant(std::type_index type, std::unique_ptr<VariantBase>&& variantValue) : _type(type) { _value = std::move(variantValue); }

    Variant& Variant::operator=(const Variant& other)
    {
        if (other._value != _value)
        {
            _type = other._type;
            _value = other._value->Clone();
        }
        return *this;
    }

    std::string to_string(const Variant& variant)
    {
        using std::to_string;
        using utilities::to_string;
        return variant._value->ToString();
    }

    std::string Variant::GetStoredTypeName() const
    {
        return _value->GetStoredTypeName();
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

    ObjectDescription Variant::GetObjectDescription() const
    {
        return _value->GetObjectDescription();
    }
}
