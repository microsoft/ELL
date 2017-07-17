////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variant.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        // VariantBase --- private implementation class used by Variant
        //
        class VariantBase
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

            virtual void ArchiveProperty(const char* name, Archiver& archiver) const = 0;
            virtual void UnarchiveProperty(const char* name, Unarchiver& archiver, SerializationContext& context) = 0;

        private:
            friend class ell::utilities::Variant;

            template <typename ValueType>
            ValueType GetValue() const;

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
            const ValueType& GetValue() const { return _value; }
            void SetValue(const ValueType& value);

            virtual std::unique_ptr<VariantBase> Clone() const override;
            virtual std::string ToString() const override;
            virtual std::string GetStoredTypeName() const override;
            virtual bool IsPrimitiveType() const override { return std::is_fundamental<ValueType>::value; }
            virtual bool IsIntegralType() const override { return std::is_integral<ValueType>::value; }
            virtual bool IsFloatingPointType() const override { return std::is_floating_point<ValueType>::value; }
            virtual bool IsEnumType() const override { return std::is_enum<ValueType>::value; }

            virtual void ParseInto(const std::string& s) override;
            virtual bool TryParseInto(const std::string& s) override;

            virtual bool IsIArchivable() const override { return !IsPrimitiveType(); }
            virtual bool IsPointer() const override { return std::is_pointer<ValueType>::value; }

            // Getting value as a fundamental type (of the largest width)
            virtual intmax_t GetIntValue() const override;
            virtual long double GetFloatValue() const override;
            virtual void SetIntValue(intmax_t value) override;
            virtual void SetFloatValue(long double value) override;

            virtual void ArchiveProperty(const char* name, Archiver& archiver) const override;
            virtual void UnarchiveProperty(const char* name, Unarchiver& archiver, SerializationContext& context) override;

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
            if(std::is_base_of<ArchivedAsPrimitive, std::decay_t<ValueType>>())
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
            return 0;
        }

        template <typename T, IsFundamental<T> = 0>
        long double CastToLongDouble(T&& value)
        {
            return static_cast<long double>(value);
        }

        template <typename T, IsNotFundamental<T> = 0>
        long double CastToLongDouble(T&& value)
        {
            return 0;
        }

        // Helper functions to allow SFINAE to select between implementations of ArchiveProperty
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ArchiveValue(Archiver& archiver, const char* name, ValueType&& value)
        {
            archiver.Archive(name, value);
        }

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void ArchiveValue(Archiver& archiver, const char* name, ValueType&& value)
        {
            archiver.Archive(name, value);
        }

        template <typename ValueType, IsNotArchivable<ValueType> concept = 0>
        void ArchiveValue(Archiver& archiver, const char* name, ValueType&& value)
        {
            throw InputException(InputExceptionErrors::typeMismatch, "VariantBase::ArchiveProperty called with unarchivable type");
        }

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void UnarchiveValue(Unarchiver& archiver, SerializationContext& context, const char* name, ValueType&& value)
        {
            archiver.Unarchive(name, value);
        }

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void UnarchiveValue(Unarchiver& archiver, SerializationContext& context, const char* name, ValueType&& value)
        {
            archiver.Unarchive(name, value);
        }

        template <typename ValueType, IsNotArchivable<ValueType> concept = 0>
        void UnarchiveValue(Unarchiver& archiver, SerializationContext& context, const char* name, ValueType&& value)
        {
            throw InputException(InputExceptionErrors::typeMismatch, "VariantBase::ArchiveProperty called with unarchivable type");
        }

        //
        // VariantBase implementation
        //
        inline VariantBase::VariantBase(std::type_index type)
            : _type(type){};

        template <typename ValueType>
        ValueType VariantBase::GetValue() const
        {
            auto thisPtr = dynamic_cast<const VariantDetail::VariantDerived<std::decay_t<ValueType>>*>(this);
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
        VariantDerived<ValueType>::VariantDerived()
            : VariantBase(typeid(ValueType)), _value(ValueType()), _typeName(TypeName<ValueType>::GetName())
        {
        }

        template <typename ValueType>
        VariantDerived<ValueType>::VariantDerived(const ValueType& val)
            : VariantBase(typeid(ValueType)), _value(val), _typeName(TypeName<ValueType>::GetName())
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
                throw(utilities::InputException(utilities::InputExceptionErrors::typeMismatch));
            }
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::SetFloatValue(long double value)
        {
            bool success = TryConvertValue(value, _value);
            if (!success)
            {
                throw(utilities::InputException(utilities::InputExceptionErrors::typeMismatch));
            }
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::ParseInto(const std::string& s)
        {
            if (!TryParseInto(s))
            {
                throw(utilities::InputException(utilities::InputExceptionErrors::typeMismatch));
            }
        }

        template <typename ValueType>
        bool VariantDerived<ValueType>::TryParseInto(const std::string& s)
        {
            return TryParseValue(s, _value);
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::ArchiveProperty(const char* name, Archiver& archiver) const
        {
            ArchiveValue(archiver, name, GetValue());
        }

        template <typename ValueType>
        void VariantDerived<ValueType>::UnarchiveProperty(const char* name, Unarchiver& archiver, SerializationContext& context)
        {
            UnarchiveValue(archiver, context, name, _value);
        }

    } // end VariantDetail namespace

    //
    // Variant implementation
    //
    template <typename ValueType, ValueType Default>
    Variant::Variant()
        : _type(std::type_index(typeid(ValueType)))
    {
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(Default);
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
    }

    template <typename ValueType>
    Variant::Variant(ValueType&& value)
        : _type(std::type_index(typeid(ValueType)))
    {
        static_assert(!std::is_same<std::decay_t<ValueType>, Variant>(), "Can't make a Variant of a Variant");
        auto derivedPtr = new VariantDetail::VariantDerived<std::decay_t<ValueType>>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantDetail::VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantDetail::VariantBase>(basePtr);
    }

    template <typename ValueType>
    ValueType Variant::GetValue() const
    {
        if (!_value)
        {
            throw InputException(InputExceptionErrors::nullReference, std::string{ "Variant::GetValue called on empty Variant" });
        }

        if (std::type_index(typeid(ValueType)) != _type)
        {
            throw InputException(InputExceptionErrors::typeMismatch, std::string{ "VariantBase::GetValue called with wrong type. Called with: " + TypeName<ValueType>::GetName() + ", but stored value is: " + GetStoredTypeName() });
        }

        return _value->GetValue<ValueType>();
    }

    template <typename ValueType>
    bool Variant::TryGetValue(ValueType& value) const
    {
        if (!IsType<ValueType>())
        {
            return false;
        }

        value = _value->GetValue<ValueType>();
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

    template <typename ValueType>
    Variant& Variant::operator=(ValueType&& value)
    {
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
}
}
