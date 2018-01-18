////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variant.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "FunctionUtils.h"
#include "IArchivable.h"
#include "JsonArchiver.h"
#include "TypeName.h"
#include "Unused.h"

// stl
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace ell
{
namespace utilities
{
    class Variant;

    namespace VariantDetail
    {
        class VariantBase;
    }

    /// <summary> Enabled if ValueType is a Variant. </summary>
    template <typename ValueType>
    using IsVariant = typename std::enable_if_t<std::is_same<std::decay_t<ValueType>, Variant>::value, bool>;

    /// <summary> Disabled if ValueType is a Variant. </summary>
    template <typename ValueType>
    using IsNotVariant = typename std::enable_if_t<!std::is_same<std::decay_t<ValueType>, Variant>::value, bool>;

    /// <summary> </summary>
    template <typename ValueType>
    using IsArchivableVariantType = typename std::enable_if_t<(std::is_base_of<IArchivable, typename std::decay<ValueType>::type>::value) ||
                                                                  (std::is_fundamental<typename std::decay<ValueType>::type>::value) ||
                                                                  (std::is_same<typename std::decay<ValueType>::type, std::string>::value),
                                                              bool>;

    // TODO: also add std::array of archivable variant types
    template <typename ValueType>
    using IsNotArchivableVariantType = typename std::enable_if_t<(!std::is_base_of<IArchivable, typename std::decay<ValueType>::type>::value) &&
                                                                     (!std::is_fundamental<typename std::decay<ValueType>::type>::value) &&
                                                                     (!std::is_same<typename std::decay<ValueType>::type, std::string>::value),
                                                                 bool>;

    /// <summary> A class that can hold any kind of value and provide a type-safe way to access it </summary>
    class Variant : public IArchivable
    {
    public:
        /// <summary> Default Constructor for an empty variant </summary>
        Variant();

        /// <summary> Constructor for default-constructed variant of the given type. </summary>
        template <typename ValueType, ValueType Default = ValueType()>
        Variant();

        /// <summary> Constructor from basic (non-variant) types. </summary>
        template <typename ValueType, IsNotVariant<ValueType> concept = true>
        explicit Variant(ValueType&& value);

        /// <summary> Copy constructor. </summary>
        Variant(const Variant& other);

        /// <summary> Move constructor. </summary>
        Variant(Variant&& other) = default;

        /// <summary> Copy assignment operator. </summary>
        Variant& operator=(const Variant& other);

        /// <summary> Move assignment operator. </summary>
        Variant& operator=(Variant&& other) = default;

        /// <summary> Assignment operator from basic (non-variant) types. </summary>
        template <typename ValueType, IsNotVariant<ValueType> concept = true>
        Variant& operator=(ValueType&& value);

        /// <summary> Gets a string representation of the value. </summary>
        ///
        /// <returns> A string representation of the value. </summary>
        std::string ToString() const;

        /// <summary> Gets the type name of the value stored in the variant. </summary>
        ///
        /// <returns> The type name of the value stored in the variant. </returns>
        std::string GetStoredTypeName() const;

        /// <summary> Get a type-safe value from the variant. </summary>
        ///
        /// <returns> The variant's current value. </returns>
        template <typename ValueType>
        const ValueType& GetValue() const;

        /// <summary> Attempt to get a type-safe value from the variant. </summary>
        ///
        /// <param name="value"> Gets the variant's current value if possible. </returns>
        /// <returns> `true` if the `value` argument was assigned. </returns>
        template <typename ValueType>
        bool TryGetValue(ValueType& value) const;

        /// <summary> Set the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        template <typename ValueType>
        void SetValue(ValueType&& value);

        /// <summary> Attempt to set a the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        template <typename ValueType>
        bool TrySetValue(ValueType&& value);

        /// <summary> Set the variant to a value regardless of its current type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        template <typename ValueType>
        void ResetValue(ValueType&& value);

        /// <summary> Get a value from the variant. </summary>
        ///
        /// <returns> The variant's current value. </returns>
        template <typename ValueType>
        ValueType GetValueAs() const;

        /// <summary> Attempt to get a value from the variant. </summary>
        ///
        /// <param name="value"> Gets the variant's current value if possible. </returns>
        /// <returns> `true` if the `value` argument was assigned. </returns>
        template <typename ValueType>
        bool TryGetValueAs(ValueType& value) const;

        /// <summary> Set the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        template <typename ValueType>
        void SetValueFrom(ValueType&& value);

        /// <summary> Set the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        template <typename ValueType>
        bool TrySetValueFrom(ValueType&& value);

        /// <summary> Set the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        bool TrySetValueFrom(const Variant& value);

        /// <summary> Set the variant to a value of the same type. </summary>
        ///
        /// <param name="value"> The value to set this Variant to </param>
        /// <returns> `true` if success, `false` if types differ. </returns>
        bool TrySetValueFrom(Variant& value);

        /// <summary> Set a Variant's value from a string (preserving its type) </summary>
        ///
        /// <param name="s"> The string to use to set the value </param>
        void ParseInto(const std::string& s);

        /// <summary> Attempt to set a Variant's value from a string (preserving its type) </summary>
        ///
        /// <param name="s"> The string to use to set the value </param>
        /// <returns> `true` if the assignment was successful </returns>
        bool TryParseInto(const std::string& s);

        /// <summary> Checks if the variant has a value assigned to it. </summary>
        ///
        /// <returns> True if the variant currently holds a value. </returns>
        bool IsEmpty() const;

        /// <summary> Checks the current type of the variant. </summary>
        ///
        /// <returns> True if the variant currently holds a value of type `ValueType`. </returns>
        template <typename ValueType>
        bool IsType() const;

        /// <summary> Checks if the variant is holding a primitive value. </summary>
        ///
        /// <returns> True if the variant currently holds a primitive value (integral or floating-point type). </returns>
        bool IsPrimitiveType() const;

        /// <summary> Checks if the variant is holding an integer value. </summary>
        ///
        /// <returns> True if the variant currently holds a primitive value. </returns>
        bool IsIntegralType() const;

        /// <summary> Checks if the variant is holding a floating point value. </summary>
        ///
        /// <returns> True if the variant currently holds a primitive value. </returns>
        bool IsFloatingPointType() const;

        /// <summary> Checks if the variant is holding a floating point value. </summary>
        ///
        /// <returns> True if the variant currently holds a primitive value. </returns>
        bool IsEnumType() const;

        /// <summary> Checks if the variant is holding an archivable object. </summary>
        ///
        /// <returns> True if the variant currently holds a archivable object. </returns>
        bool IsIArchivable() const;

        /// <summary> Checks if the variant is holding a pointer. </summary>
        ///
        /// <returns> True if the variant currently holds a pointer. </returns>
        bool IsPointer() const;

        /// <summary> Checks if two variants hold the same type of value </summary>
        bool IsSameTypeAs(const Variant& other) const;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "Variant"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Helpful operator overloads for variants </summary>
        void operator++(); // Prefix increment
        void operator++(int); // Postfix increment
        void operator--(); // Prefix decrement
        void operator--(int); // Postfix decrement

    protected:
        void WriteToArchive(Archiver& archiver) const override;
        void ReadFromArchive(Unarchiver& archiver) override;

    private:
        friend std::string to_string(const Variant& variant);
        friend class IArchivable;

        template <typename ValueType, typename... Args>
        friend Variant MakeVariant(Args&&... args);

        Variant(std::type_index type, std::unique_ptr<VariantDetail::VariantBase> variantValue);
        void RegisterArchivableVariantTypes(VariantTypeRegistry& registry);
        template <typename ValueType>
        void RegisterArchivableVariantType(VariantTypeRegistry& registry);
        template <typename ValueType>
        void RegisterArchivableVariantVectorType(VariantTypeRegistry& registry);
    
        VariantDetail::VariantBase* GetBasePointer() { return _value.get(); }
        const VariantDetail::VariantBase* GetBasePointer() const { return _value.get(); }

        std::type_index _type;
        std::unique_ptr<VariantDetail::VariantBase> _value;
    };

    /// <summary> Convenience function to create a Variant </summary>
    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args&&... args);

    /// <summary> Get string representation of a Variant </summary>
    std::string to_string(const Variant& variant);

    //
    // Helper functions for calling functions with vectors of Variants
    //

    // <summary> Returns a vector of default-initialized variants of the types given in TupleType </summary>
    template <typename TupleType>
    std::vector<Variant> GetVariantsFromTupleType();

    // <summary> Returns a vector of default-initialized variants of the types needed to call a function of the type FunctionType </summary>
    template <typename FunctionType>
    std::vector<Variant> GetVariantsFromFunctionArgs();

    // <summary> Returns a vector of default-initialized variants of the types needed to call a function of the given function </summary>
    template <typename FunctionType>
    std::vector<Variant> GetVariantsFromFunctionArgs(FunctionType& f);

    /// <summary> Fills in tuple with values taken from vector of Variants </summary>
    template <typename ArgsTupleType>
    ArgsTupleType GetTupleFromVariants(const std::vector<Variant>& args);

    /// <summary> Fills in tuple with values taken from vector of Variants </summary>
    template <typename FunctionType>
    FunctionArgTypes<FunctionType> GetArgTupleFromVariants(FunctionType& function, const std::vector<Variant>& args);

    /// <summary> Call a function with a vector of Variants </summary>
    template <typename FunctionType>
    FunctionReturnType<FunctionType> CallFunctionWithVariants(FunctionType& function, const std::vector<utilities::Variant>& args);
}
}

#include "../tcc/Variant.tcc"
