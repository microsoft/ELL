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
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // Private code we'd like to hide:
    //
    namespace VariantDetail
    {
        //
        // Operator helpers
        //
        template <typename ValueType>
        using IsIncrementable = typename std::enable_if_t<(std::is_integral<std::decay_t<ValueType>>::value && !std::is_same<std::decay_t<ValueType>, bool>::value) || std::is_floating_point<std::decay_t<ValueType>>::value, bool>;
        template <typename ValueType>
        using IsNotIncrementable = typename std::enable_if_t<!((std::is_integral<std::decay_t<ValueType>>::value && !std::is_same<std::decay_t<ValueType>, bool>::value) || std::is_floating_point<std::decay_t<ValueType>>::value), bool>;

        template <typename ValueType, IsIncrementable<ValueType> concept = true>
        void Increment(ValueType& value)
        {
            ++value;
        }

        template <typename ValueType, IsNotIncrementable<ValueType> concept = true>
        void Increment(ValueType& value)
        {
            UNUSED(value);
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        };

        template <typename ValueType, IsIncrementable<ValueType> concept = true>
        void Decrement(ValueType& value)
        {
            --value;
        }

        template <typename ValueType, IsNotIncrementable<ValueType> concept = true>
        void Decrement(ValueType& value)
        {
            UNUSED(value);
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        //
        // VariantBase --- private implementation class used by Variant
        //
        class VariantBase : public ArchivedAsPrimitive
        {
        public:
            virtual ~VariantBase() = default;

        protected:
            VariantBase(std::type_index type);
            virtual std::unique_ptr<VariantBase> Clone() const = 0;
            virtual std::string ToString() const = 0;
            virtual std::string GetStoredTypeName() const = 0;
            virtual bool IsPrimitiveType() const = 0;
            virtual bool IsIntegralType() const = 0;
            virtual bool IsFloatingPointType() const = 0;
            virtual bool IsEnumType() const = 0;

            virtual void ParseInto(const std::string& s) = 0;
            virtual bool TryParseInto(const std::string& s) = 0;

            virtual bool IsIArchivable() const = 0;
            virtual bool IsPointer() const = 0;

            // Getting / setting value as a fundamental type (of the largest width)
            virtual intmax_t GetIntValue() const = 0;
            virtual long double GetFloatValue() const = 0;
            virtual void SetIntValue(intmax_t value) = 0;
            virtual void SetFloatValue(long double value) = 0;
            static std::string GetTypeName() { return "VariantBase"; }
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            // operators
            virtual void operator++() { throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch); };
            virtual void operator++(int) { throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch); };
            virtual void operator--() { throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch); };
            virtual void operator--(int) { throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch); };

        private:
            friend class ell::utilities::Variant;

            template <typename ValueType>
            ValueType& GetValue();

            template <typename ValueType>
            const ValueType& GetValue() const;

            template <typename ValueType>
            void SetValue(ValueType&& value);

            std::type_index _type; // redundant with type in Variant class.
        };

        //
        // VariantDerived definition --- private implementation class used by Variant
        //
        template <typename ValueType>
        class VariantDerived : public VariantBase
        {
        public:
            /// <summary> Default constructor </summary>
            VariantDerived();

            /// <summary> Constructor </summary>
            ///
            /// <param name="val"> Value to wrap in a variant </param>
            VariantDerived(const ValueType& val);

        protected:
            ValueType& GetValue() { return _value; }
            const ValueType& GetValue() const { return _value; }
            void SetValue(const ValueType& value);

            std::unique_ptr<VariantBase> Clone() const override;
            std::string ToString() const override;
            std::string GetStoredTypeName() const override;
            bool IsPrimitiveType() const override { return std::is_fundamental<ValueType>::value; }
            bool IsIntegralType() const override { return std::is_integral<ValueType>::value; }
            bool IsFloatingPointType() const override { return std::is_floating_point<ValueType>::value; }
            bool IsEnumType() const override { return std::is_enum<ValueType>::value; }

            void ParseInto(const std::string& s) override;
            bool TryParseInto(const std::string& s) override;

            bool IsIArchivable() const override { return !IsPrimitiveType(); }
            bool IsPointer() const override { return std::is_pointer<ValueType>::value; }

            // Getting value as a fundamental type (of the largest width)
            intmax_t GetIntValue() const override;
            long double GetFloatValue() const override;
            void SetIntValue(intmax_t value) override;
            void SetFloatValue(long double value) override;

            void operator++() override { Increment(_value); }
            void operator++(int) override { Increment(_value); }
            void operator--() override { Decrement(_value); }
            void operator--(int) override { Decrement(_value); }

            static std::string GetTypeName() { return GetCompositeTypeName<ValueType>("VariantDerived"); }
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            friend class Variant;
            friend class VariantBase;

            ValueType _value;
            std::string _typeName;
        };

        // GetValueString
        using std::to_string;
        using utilities::to_string;
        inline std::string to_string(const std::string& str)
        {
            return str;
        }

        template <typename ValueType>
        auto GetValueStringHelper(const ValueType& value, ...) -> std::string
        {
            UNUSED(value);
            return "";
        }

        template <typename ValueType>
        auto GetValueStringHelper(const ValueType& value, int) -> decltype(to_string(value), std::string())
        {
            return to_string(value);
        }

        template <typename ValueType>
        auto GetValueString(const ValueType& value) -> std::string
        {
            return GetValueStringHelper(value, 0);
        }

        // TryParseValue

        template <typename ValueType>
        bool TryParseValueHelper(const std::string& s, ValueType& value, ...)
        {
            UNUSED(s);
            UNUSED(value);
            return false;
        }

        inline bool TryParseValueHelper(const std::string& s, std::string& value, int)
        {
            value = s;
            return true;
        }

        template <typename ValueType, IsFundamental<ValueType> = true>
        bool TryParseValueHelper(const std::string& s, ValueType& value, int)
        {
            std::stringstream sstr(s);
            ValueType v;
            sstr >> v;
            value = v;
            return !sstr.fail();
        }

        template <typename ValueType, IsEnum<ValueType> = true>
        bool TryParseValueHelper(const std::string& s, ValueType& value, int)
        {
            std::stringstream sstr(s);
            std::underlying_type_t<ValueType> v;
            sstr >> v;
            value = static_cast<ValueType>(v);
            return !sstr.fail();
        }

        template <typename ValueType>
        bool TryParseArchivable(std::string s, ValueType& value)
        {
            if (std::is_base_of<ArchivedAsPrimitive, std::decay_t<ValueType>>())
            {
                s = std::string("\"") + s + std::string("\"");
            }

            std::stringstream stream(s);
            SerializationContext context;
            JsonUnarchiver ar(stream, context);
            ar >> value;
            return true;
        }

        template <typename ValueType, IsIArchivable<ValueType> = true>
        bool TryParseValueHelper(const std::string& s, ValueType& value, int)
        {
            return TryParseArchivable(s, value);
        }

        template <typename ValueType, IsVector<ValueType> = true>
        bool TryParseValueHelper(const std::string& s, ValueType& value, int)
        {
            return TryParseArchivable(s, value);
        }

        template <typename ValueType>
        bool TryParseValue(const std::string& s, ValueType& value)
        {
            return TryParseValueHelper(s, value, 0);
        }

        // TryConvertValue
        template <typename InputValueType, typename OutputValueType>
        bool TryConvertValueHelper(const InputValueType& input, OutputValueType& output, ...)
        {
            UNUSED(input, output);
            return false;
        }

        inline bool TryConvertValueHelper(const std::string& inValue, std::string& outValue, int)
        {
            outValue = inValue;
            return true;
        }

        // int, float, enum, bool <--> int, float, enum, bool  (maybe not enum<-->float)

        // fundamental -> int
        template <typename InputValueType, typename OutputValueType, IsFundamental<InputValueType> = true, IsNonBooleanIntegral<OutputValueType> = true>
        bool TryConvertValueHelper(const InputValueType& inValue, OutputValueType& outValue, int)
        {
            outValue = static_cast<OutputValueType>(inValue);
            return true;
        }

        // fundamental -> bool
        template <typename InputValueType, typename OutputValueType, IsFundamental<InputValueType> = true, IsBoolean<OutputValueType> = true>
        bool TryConvertValueHelper(const InputValueType& inValue, OutputValueType& outValue, int)
        {
            outValue = static_cast<OutputValueType>(inValue != 0);
            return true;
        }

        // fundamental -> float
        template <typename InputValueType, typename OutputValueType, IsFundamental<InputValueType> = true, IsFloatingPoint<OutputValueType> = true>
        bool TryConvertValueHelper(const InputValueType& inValue, OutputValueType& outValue, int)
        {
            outValue = static_cast<OutputValueType>(inValue);
            return true;
        }

        // int -> enum
        template <typename InputValueType, typename OutputValueType, IsIntegral<InputValueType> = true, IsEnum<OutputValueType> = true>
        bool TryConvertValueHelper(const InputValueType& inValue, OutputValueType& outValue, int)
        {
            outValue = static_cast<OutputValueType>(inValue);
            return true;
        }

        // main function
        template <typename InputValueType, typename OutputValueType>
        bool TryConvertValue(const InputValueType& inValue, OutputValueType& outValue)
        {
            return TryConvertValueHelper(inValue, outValue, 0);
        }

        // Setting values from fundamental types
        template <typename ValueType>
        using CanCastToInt = std::enable_if_t<std::is_fundamental<std::decay_t<ValueType>>::value || std::is_enum<ValueType>::value, bool>;

        template <typename ValueType>
        using CanNotCastToInt = std::enable_if_t<!(std::is_fundamental<std::decay_t<ValueType>>::value || std::is_enum<ValueType>::value), bool>;

        template <typename T, CanCastToInt<T> = true>
        intmax_t CastToIntMax(T&& value)
        {
            return static_cast<intmax_t>(value);
        }

        template <typename T, CanNotCastToInt<T> = true>
        intmax_t CastToIntMax(T&& value)
        {
            UNUSED(value);
            return 0;
        }

        template <typename T, IsFundamental<T> = true>
        long double CastToLongDouble(T&& value)
        {
            return static_cast<long double>(value);
        }

        template <typename T, IsNotFundamental<T> = true>
        long double CastToLongDouble(T&& value)
        {
            UNUSED(value);
            return 0;
        }

        // Helper functions to allow SFINAE to select between implementations of Archiver::operator<<
        template <typename ValueType, IsArchivableVariantType<ValueType> concept = true>
        void ArchiveValue(Archiver& archiver, ValueType&& value)
        {
            archiver << value;
        }

        template <typename ValueType, IsArchivableVariantType<ValueType> concept = true>
        void ArchiveValue(Archiver& archiver, const std::vector<ValueType>& value)
        {
            archiver << value;
        }

        template <typename ValueType, IsNotArchivableVariantType<ValueType> concept = true, IsNotVector<ValueType> concept2 = true>
        void ArchiveValue(Archiver& archiver, ValueType&& value)
        {
            UNUSED(archiver, value);
            throw InputException(InputExceptionErrors::typeMismatch, std::string("VariantBase::ArchiveValue called with unarchivable type: ") + GetTypeName<ValueType>());
        }

        template <typename ValueType, IsNotArchivableVariantType<ValueType> concept = true>
        void ArchiveValue(Archiver& archiver, const std::vector<ValueType>& value)
        {
            UNUSED(archiver, value);
            throw InputException(InputExceptionErrors::typeMismatch, std::string("VariantBase::ArchiveValue called with unarchivable vector type: ") + GetTypeName<ValueType>());
        }

        //
        // Helper functions to allow SFINAE to select between implementations of Unarchiver::operator>>
        //
        template <typename ValueType, IsArchivableVariantType<ValueType> concept = true>
        void UnarchiveValue(Unarchiver& archiver, ValueType&& value)
        {
            archiver >> value;
        }

        template <typename ValueType, IsArchivableVariantType<ValueType> concept = true>
        void UnarchiveValue(Unarchiver& archiver, std::vector<ValueType>& value)
        {
            archiver >> value;
        }

        template <typename ValueType, IsNotArchivableVariantType<ValueType> concept = true, IsNotVector<ValueType> concept2 = true>
        void UnarchiveValue(Unarchiver& archiver, ValueType&& value)
        {
            UNUSED(archiver, value);
            throw InputException(InputExceptionErrors::typeMismatch, std::string("VariantBase::ArchiveValue called with unarchivable type: ") + GetTypeName<ValueType>());
        }

        template <typename ValueType, IsNotArchivableVariantType<ValueType> concept = true>
        void UnarchiveValue(Unarchiver& archiver, std::vector<ValueType>& value)
        {
            UNUSED(archiver, value);
            throw InputException(InputExceptionErrors::typeMismatch, std::string("VariantBase::UnarchiveValue called with unarchivable type: ") + GetTypeName<typename std::decay<ValueType>::type>());
        }

        //
        // VariantBase implementation
        //
        inline VariantBase::VariantBase(std::type_index type) :
            _type(type){};

        template <typename ValueType>
        ValueType& VariantBase::GetValue()
        {
            auto thisPtr = dynamic_cast<const VariantDetail::VariantDerived<std::decay_t<ValueType>>*>(this);
            if (thisPtr == nullptr)
            {
                throw InputException(InputExceptionErrors::typeMismatch, std::string{ "VariantBase::GetValue called with wrong type. Called with: " + TypeName<ValueType>::GetName() + ", but stored value is: " + GetStoredTypeName() });
            }

            return thisPtr->GetValue();
        }

        template <typename ValueType>
        const ValueType& VariantBase::GetValue() const
        {
            const auto thisPtr = dynamic_cast<const VariantDetail::VariantDerived<std::decay_t<ValueType>>*>(this);
            if (thisPtr == nullptr)
            {
                throw InputException(InputExceptionErrors::typeMismatch, std::string{ "VariantBase::GetValue called with wrong type. Called with: " + TypeName<ValueType>::GetName() + ", but stored value is: " + GetStoredTypeName() });
            }

            return thisPtr->GetValue();
        }

        template <typename ValueType>
        void VariantBase::SetValue(ValueType&& value)
        {
            auto thisPtr = dynamic_cast<VariantDetail::VariantDerived<std::decay_t<ValueType>>*>(this);
            if (thisPtr == nullptr)
            {
                throw InputException(InputExceptionErrors::typeMismatch, std::string{ "VariantBase::SetValue called with wrong type. Type: " + TypeName<ValueType>::GetName() });
            }
            thisPtr->SetValue(value);
        }

        //
        // VariantDerived implementation
        //
        template <typename ValueType>
        VariantDerived<ValueType>::VariantDerived() :
            VariantBase(typeid(ValueType)),
            _value(ValueType()),
            _typeName(TypeName<ValueType>::GetName())
        {
        }

        template <typename ValueType>
        VariantDerived<ValueType>::VariantDerived(const ValueType& val) :
            VariantBase(typeid(ValueType)),
            _value(val),
            _typeName(TypeName<ValueType>::GetName())
        {
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::SetValue(const ValueType& value)
        {
            _value = value;
        }

        template <typename ValueType>
        std::unique_ptr<VariantBase> VariantDerived<ValueType>::Clone() const
        {
            auto ptr = static_cast<VariantBase*>(new VariantDerived<ValueType>(_value));
            return std::unique_ptr<VariantBase>(ptr);
        }

        template <typename ValueType>
        std::string VariantDerived<ValueType>::ToString() const
        {
            return GetValueString(_value);
        }

        template <typename ValueType>
        std::string VariantDerived<ValueType>::GetStoredTypeName() const
        {
            return TypeName<std::decay_t<ValueType>>::GetName();
        }

        template <typename ValueType>
        intmax_t VariantDerived<ValueType>::GetIntValue() const
        {
            return CastToIntMax(_value);
        }

        template <typename ValueType>
        long double VariantDerived<ValueType>::GetFloatValue() const
        {
            return CastToLongDouble(_value);
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::SetIntValue(intmax_t value)
        {
            bool success = TryConvertValue(value, _value);
            if (!success)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::SetFloatValue(long double value)
        {
            bool success = TryConvertValue(value, _value);
            if (!success)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::ParseInto(const std::string& s)
        {
            if (!TryParseInto(s))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }
        }

        template <typename ValueType>
        bool VariantDerived<ValueType>::TryParseInto(const std::string& s)
        {
            return TryParseValue(s, _value);
        }

        // template <typename ValueType>
        // void VariantDerived<ValueType>::operator++()
        // {
        // }

        // template <typename ValueType>
        // void VariantDerived<ValueType>::operator++(int)
        // {
        // }

        // template <typename ValueType>
        // void VariantDerived<ValueType>::operator--()
        // {
        // }

        // template <typename ValueType>
        // void VariantDerived<ValueType>::operator--(int)
        // {
        // }

        template <typename ValueType>
        void VariantDerived<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            ArchiveValue(archiver, GetValue());
            // archiver << _value;
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            UnarchiveValue(archiver, _value);
            // archiver >> _value;
        }

    } // namespace VariantDetail

    //
    // Variant implementation
    //
    template <typename ValueType, ValueType Default>
    Variant::Variant() :
        _type(std::type_index(typeid(ValueType)))
    {
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(Default);
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
    }

    template <typename ValueType, IsNotVariant<ValueType> concept>
    Variant::Variant(ValueType&& value) :
        _type(std::type_index(typeid(ValueType)))
    {
        static_assert(!std::is_same<std::decay_t<ValueType>, Variant>(), "Can't make a Variant of a Variant");
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
    }

    template <typename ValueType>
    const ValueType& Variant::GetValue() const
    {
        if (!_value)
        {
            throw InputException(InputExceptionErrors::nullReference, std::string{ "Variant::GetValue called on empty Variant" });
        }

        if (std::type_index(typeid(ValueType)) != _type)
        {
            throw InputException(InputExceptionErrors::typeMismatch, std::string{ "VariantBase::GetValue called with wrong type. Called with: " + TypeName<ValueType>::GetName() + ", but stored value is: " + GetStoredTypeName() });
        }

        return GetBasePointer()->GetValue<ValueType>();
    }

    template <typename ValueType>
    bool Variant::TryGetValue(ValueType& value) const
    {
        if (!IsType<ValueType>())
        {
            return false;
        }

        value = GetBasePointer()->GetValue<ValueType>();
        return true;
    }

    template <typename ValueType>
    void Variant::SetValue(ValueType&& value)
    {
        if (!TrySetValue(value))
        {
            throw InputException(InputExceptionErrors::typeMismatch, std::string{ "Variant::SetValue called with wrong type. Type: " + TypeName<ValueType>::GetName() });
        }
    }

    template <typename ValueType>
    bool Variant::TrySetValue(ValueType&& value)
    {
        if (!IsType<std::decay_t<ValueType>>())
        {
            return false;
        }

        _value->SetValue(value);
        return true;
    }

    template <typename ValueType>
    void Variant::ResetValue(ValueType&& value)
    {
        static_assert(!std::is_same<std::decay_t<ValueType>, Variant>(), "Can't make a Variant of a Variant");

        _type = std::type_index(typeid(ValueType));
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
    }

    template <typename ValueType>
    ValueType Variant::GetValueAs() const
    {
        ValueType result;
        if (!TryGetValueAs<ValueType>(result))
        {
            throw InputException(InputExceptionErrors::typeMismatch, "Could not cast Variant value to given type");
        }
        return result;
    }

    template <typename ValueType>
    bool Variant::TryGetValueAs(ValueType& value) const
    {
        if (_value == nullptr)
        {
            return false;
        }

        if (IsType<ValueType>())
        {
            return TryGetValue<ValueType>(value);
        }
        else if (std::is_same<ValueType, std::string>())
        {
            return VariantDetail::TryConvertValue(_value->ToString(), value);
        }
        else if (IsIntegralType())
        {
            return VariantDetail::TryConvertValue(_value->GetIntValue(), value);
        }
        else if (IsEnumType())
        {
            return VariantDetail::TryConvertValue(_value->GetIntValue(), value);
        }
        else if (IsFloatingPointType())
        {
            return VariantDetail::TryConvertValue(_value->GetFloatValue(), value);
        }
        return TryGetValue<ValueType>(value);
    }

    template <typename ValueType>
    void Variant::SetValueFrom(ValueType&& value)
    {
        if (!TrySetValueFrom(value))
        {
            throw InputException(InputExceptionErrors::typeMismatch, "Could not set Variant value from given type");
        }
    }

    template <typename ValueType>
    bool Variant::TrySetValueFrom(ValueType&& value)
    {
        using std::to_string;
        using utilities::to_string;

        static_assert(!std::is_same<std::decay_t<ValueType>, Variant>(), "Can't set value from a Variant");

        if (_value == nullptr)
        {
            return false;
        }

        if (IsType<ValueType>())
        {
            return TrySetValue(value);
        }
        else if (IsType<std::string>())
        {
            return TrySetValue(to_string(value));
        }
        else if (IsIntegralType())
        {
            // If we are integral, we can accept integral, enum, or floating-point types
            if (std::is_fundamental<std::decay_t<ValueType>>::value || std::is_enum<std::decay_t<ValueType>>::value)
            {
                _value->SetIntValue(VariantDetail::CastToIntMax(value));
                return true;
            }
            return false;
        }
        else if (IsEnumType())
        {
            // If we are integral, we can accept integral or enum types
            if (std::is_integral<std::decay_t<ValueType>>::value || std::is_enum<std::decay_t<ValueType>>::value)
            {
                _value->SetIntValue(VariantDetail::CastToIntMax(value));
                return true;
            }
            return false;
        }
        else if (IsFloatingPointType())
        {
            // If we are integral, we can accept integral or floating-point types
            if (std::is_fundamental<std::decay_t<ValueType>>::value)
            {
                _value->SetFloatValue(VariantDetail::CastToLongDouble(value));
                return true;
            }
        }
        return TrySetValue(value);
    }

    inline bool Variant::TrySetValueFrom(Variant& other)
    {
        return TrySetValueFrom(static_cast<const Variant&>(other));
    }

    inline bool Variant::TrySetValueFrom(const Variant& other)
    {
        if (IsSameTypeAs(other))
        {
            if (other._value)
            {
                _value = other._value->Clone();
            }
            else
            {
                _value = nullptr;
            }
            return true;
        }

        if ((IsIntegralType() || IsEnumType()) && (other.IsIntegralType() || other.IsEnumType()))
        {
            _value->SetIntValue(other._value->GetIntValue());
            return true;
        }
        else if ((IsIntegralType() || IsEnumType()) && (other.IsFloatingPointType()))
        {
            _value->SetIntValue(static_cast<intmax_t>(other._value->GetFloatValue()));
            return true;
        }
        else if ((IsFloatingPointType()) && (other.IsIntegralType() || other.IsEnumType()))
        {
            _value->SetFloatValue(static_cast<long double>(other._value->GetIntValue()));
            return true;
        }
        else if ((IsFloatingPointType()) && (other.IsFloatingPointType()))
        {
            _value->SetFloatValue(other._value->GetFloatValue());
            return true;
        }

        return false;
    }

    template <typename ValueType, IsNotVariant<ValueType> concept>
    Variant& Variant::operator=(ValueType&& value)
    {
        static_assert(!std::is_same<std::decay_t<ValueType>, Variant>(), "Can't make a Variant of a Variant");
        _type = std::type_index(typeid(ValueType));
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
        return *this;
    }

    template <typename ValueType>
    bool Variant::IsType() const
    {
        return (_value != nullptr && std::type_index(typeid(ValueType)) == _type);
    }

    template <typename ValueType>
    void Variant::RegisterArchivableVariantType(VariantTypeRegistry& registry)
    {
        registry.SetVariantTypeFunction<ValueType>([](Variant& variant) {
            variant.ResetValue<ValueType>(ValueType());
        });
    }

    template <typename ValueType>
    void Variant::RegisterArchivableVariantVectorType(VariantTypeRegistry& registry)
    {
        using VectorType = std::vector<ValueType>;
        registry.SetVariantTypeFunction<VectorType>([](Variant& variant) {
            variant.ResetValue<VectorType>(VectorType());
        });
    }

    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args&&... args)
    {
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(std::forward<Args>(args)...);
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        return Variant(std::type_index(typeid(ValueType)), std::unique_ptr<VariantDetail::VariantBase>(basePtr));
    }

    //
    // Helper functions
    //

    // GetTupleFromVariants
    template <typename ArgsTupleType, size_t... Sequence>
    ArgsTupleType GetArgTupleFromVariantsHelper(const std::vector<utilities::Variant>& args, std::index_sequence<Sequence...>)
    {
        return ArgsTupleType({ args[Sequence].GetValue<typename std::tuple_element<Sequence, ArgsTupleType>::type>() }...);
    }

    /// Fills in tuple with values taken from vector of Variants
    template <typename ArgsTupleType>
    ArgsTupleType GetTupleFromVariants(const std::vector<Variant>& args)
    {
        return GetArgTupleFromVariantsHelper<ArgsTupleType>(args, std::make_index_sequence<std::tuple_size<ArgsTupleType>::value>());
    }

    template <typename FunctionType>
    utilities::FunctionArgTypes<FunctionType> GetArgTupleFromVariants(FunctionType& function, const std::vector<utilities::Variant>& args)
    {
        using ArgTypes = utilities::FunctionArgTypes<FunctionType>;
        return GetArgTupleFromVariantsHelper<ArgTypes>(args);
    }

    // GetVariantsFromTupleType
    template <typename ValueTupleType, size_t... Sequence>
    std::vector<utilities::Variant> GetTupleVariantTypesHelper(std::index_sequence<Sequence...>)
    {
        return { utilities::MakeVariant<std::tuple_element_t<Sequence, ValueTupleType>>()... };
    }

    template <typename ValueTupleType>
    std::vector<utilities::Variant> GetVariantsFromTupleType()
    {
        return GetTupleVariantTypesHelper<ValueTupleType>(std::make_index_sequence<std::tuple_size<ValueTupleType>::value>{});
    }

    // GetVariantsFromFunctionArgs
    template <typename FunctionType>
    std::vector<utilities::Variant> GetVariantsFromFunctionArgs()
    {
        return GetVariantsFromTupleType<utilities::FunctionArgTypes<FunctionType>>();
    }

    template <typename FunctionType>
    std::vector<utilities::Variant> GetVariantsFromFunctionArgs(FunctionType&)
    {
        return GetVariantsFromTupleType<utilities::FunctionArgTypes<FunctionType>>();
    }

    template <typename FunctionType, size_t... Sequence>
    auto CallFunctionWithVariantsHelper(FunctionType& function, const std::vector<utilities::Variant>& args, std::index_sequence<Sequence...>) -> FunctionReturnType<FunctionType>
    {
        auto argsTuple = GetTupleFromVariants(function, args);
        return function(std::get<Sequence>(argsTuple)...);
    }

    template <typename FunctionType>
    auto CallFunctionWithVariants(FunctionType& function, const std::vector<utilities::Variant>& args) -> FunctionReturnType<FunctionType>
    {
        using ArgTypes = utilities::FunctionArgTypes<FunctionType>;
        return CallFunctionWithVariantsHelper(function, args, std::make_index_sequence<std::tuple_size<ArgTypes>::value>());
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation
