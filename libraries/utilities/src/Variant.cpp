////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variant.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variant.h"

namespace ell
{
namespace utilities
{
    //
    // Variant implementation
    //
    Variant::Variant()
        : _type(std::type_index(typeid(void)))
    {
        _value = nullptr;
    }

    Variant::Variant(const Variant& other)
        : _type(other._type)
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

    Variant::Variant(std::type_index type, std::unique_ptr<VariantDetail::VariantBase> variantValue)
        : _type(type)
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

    void Variant::ParseInto(const std::string& s)
    {
        _value->ParseInto(s);
    }

    bool Variant::TryParseInto(const std::string& s)
    {
        return _value->TryParseInto(s);
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

    bool Variant::IsIntegralType() const
    {
        return _value->IsIntegralType();
    }

    bool Variant::IsFloatingPointType() const
    {
        return _value->IsFloatingPointType();
    }

    bool Variant::IsEnumType() const
    {
        return _value->IsEnumType();
    }

    bool Variant::IsIArchivable() const
    {
        return _value->IsIArchivable();
    }

    bool Variant::IsPointer() const
    {
        return _value->IsPointer();
    }

    bool Variant::IsSameTypeAs(const Variant& other) const
    {
        return other._type == _type;
    }

    void Variant::ArchiveProperty(const char* name, Archiver& archiver) const
    {
        _value->ArchiveProperty(name, archiver);
    }

    void Variant::UnarchiveProperty(const char* name, Unarchiver& archiver, SerializationContext& context)
    {
        _value->UnarchiveProperty(name, archiver, context);
    }

    std::string to_string(const Variant& variant)
    {
        return variant.ToString();
    }
}
}
