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

    void Variant::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["type"] << GetStoredTypeName();
        archiver["value"] << *_value;
    }

    void Variant::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        auto& context = archiver.GetContext();
        auto& registry = context.GetVariantTypeRegistry();
        if (registry.IsEmpty())
        {
            RegisterArchivableVariantTypes(registry);
        }

        std::string type;
        archiver["type"] >> type;
        registry.SetVariantType(*this, type);
        archiver["value"] >> *_value;

        assert(_value);
    }

    void Variant::RegisterArchivableVariantTypes(VariantTypeRegistry& registry)
    {
        // Basic types
        RegisterArchivableVariantType<bool>(registry);
        RegisterArchivableVariantType<int>(registry);
        RegisterArchivableVariantType<int32_t>(registry);
        RegisterArchivableVariantType<int64_t>(registry);
        RegisterArchivableVariantType<float>(registry);
        RegisterArchivableVariantType<double>(registry);
        RegisterArchivableVariantType<std::string>(registry);

        // Vectors of basic types
        RegisterArchivableVariantVectorType<bool>(registry);
        RegisterArchivableVariantVectorType<int>(registry);
        RegisterArchivableVariantVectorType<int32_t>(registry);
        RegisterArchivableVariantVectorType<int64_t>(registry);
        RegisterArchivableVariantVectorType<float>(registry);
        RegisterArchivableVariantVectorType<double>(registry);
        RegisterArchivableVariantVectorType<std::string>(registry);
    }

    //
    // Operators
    //
    void Variant::operator++() // Prefix increment
    {
        _value->operator++();
    }

    void Variant::operator++(int) // Postfix increment
    {
        _value->operator++(0);
    }

    void Variant::operator--() // Prefix decrement
    {
        _value->operator--();
    }

    void Variant::operator--(int) // Postfix decrement
    {
        _value->operator--(0);
    }

    //
    // Functions
    //
    std::string to_string(const Variant& variant)
    {
        return variant.ToString();
    }
}
}
