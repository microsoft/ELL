////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Exception.h"
#include "TypeName.h"

// stl
#include <memory>
#include <typeindex>
#include <string>
#include <type_traits>
#include <utility>

class ISerializable;

/// <summary> utilities namespace </summary>
namespace utilities
{
    class ObjectDescription;
    class VariantBase;

    /// <summary> A class that can hold any kind of value and provide a type-safe way to access it
    class Variant
    {
    public:
        /// <summary> Copy constructor </summary>
        Variant(const Variant& other);
        Variant(Variant&& other) = default;

        /// <summary> Copy assignment operator </summary>
        Variant& operator=(const Variant& other);
        Variant& operator=(Variant&& other) = default;

        /// <summary> Assignment operator from basic (non-variant) types </summary>
        template <typename ValueType>
        Variant& operator=(ValueType&& value);

        /// <summary> Get a type-safe value from the variant. </summary>
        ///
        /// <returns> The variant's current value
        template <typename ValueType>
        ValueType GetValue() const;
    
        /// <summary> Checks the current type of the variant </summary>
        ///
        /// <returns> True if the variant currently holds a value of type `ValueType` </returns>
        template <typename ValueType>
        bool IsType() const;

        bool IsPrimitiveType() const;

        bool IsSerializable() const;

        bool IsPointer() const;

        std::string GetStoredTypeName() const;

        void SetObjectDescription(ObjectDescription& description) const;

    private:
        friend std::string to_string(const Variant& variant);

        template <typename ValueType, typename... Args>
        friend Variant MakeVariant(Args&&... args);

        Variant(std::type_index type, std::unique_ptr<VariantBase>&& variantValue);

        std::type_index _type;
        std::unique_ptr<VariantBase> _value;
    };

    /// <summary> Convenience function to create a Variant </summary>
    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args&&... args);

    /// <summary> Get string representation of a Variant </summary>
    std::string to_string(const Variant& variant);
}

#include "VariantBase.h"
#include "VariantDerived.h"
#include "../tcc/Variant.tcc"
