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
    class VariantBase;

    /// <summary> A class that can hold any kind of value and provide a type-safe way to access it
    class Variant
    {
    public:
        /// <summary> Copy constructor </summary>
        Variant(const Variant& other);
        Variant(Variant&& other) = default;

        Variant(std::type_index type, std::unique_ptr<VariantBase>&& variantValue);

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

    private:
        std::type_index _type;
        std::unique_ptr<VariantBase> _value;

        // efficiency hack:
        // for often-used simple types, have a union of them and use that instead of a pointer
        /*
        union
        {
            bool boolValue;
            int32_t int32Value;
            // ...
        } _simpleValue;
        */
    };

    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args... args);
}

#include "../tcc/Variant.tcc"
