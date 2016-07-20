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
}
