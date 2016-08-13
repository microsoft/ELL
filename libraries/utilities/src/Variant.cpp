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

    std::string Variant::ToString() const
    {
        using std::to_string;
        using utilities::to_string;
        return _value->ToString();
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

    std::string to_string(const Variant& variant)
    {
        return variant.ToString();
    }
}
