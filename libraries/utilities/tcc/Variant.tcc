////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // VariantBase implementation
    //
    template <typename ValueType>
    ValueType VariantBase::GetValue() const
    {
        auto thisPtr = dynamic_cast<const VariantDerived<ValueType>*>(this);
        if (thisPtr == nullptr)
        {
            throw InputException(InputExceptionErrors::typeMismatch, "Variant::GetValue called with wrong type.");
        }

        return thisPtr->GetValue();
    }

    //
    // VariantDerived implementation
    //

    template <typename ValueType>
    VariantDerived<ValueType>::VariantDerived(const ValueType& val) : VariantBase(typeid(ValueType)), _value(val) 
    {}

    template <typename ValueType>
    std::unique_ptr<VariantBase> VariantDerived<ValueType>::Clone() const
    {
        auto ptr = static_cast<VariantBase*>(new VariantDerived<ValueType>(_value));
        return std::unique_ptr<VariantBase>(ptr);
    }

    //
    // Variant implementation
    //
    template <typename ValueType>
    Variant::Variant(ValueType&& value) : _type(std::type_index(typeid(ValueType)))
    {
        auto derivedPtr = new VariantDerived<ValueType>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantBase>(basePtr);
    }

    template <typename ValueType>
    ValueType Variant::GetValue() const
    {
        if (std::type_index(typeid(ValueType)) != _type)
        {
            throw std::runtime_error("Bad variant access");
        }

        return _value->GetValue<ValueType>();
    }

    template <typename ValueType>
    Variant& Variant::operator=(ValueType&& value)
    {
        _type = std::type_index(typeid(ValueType));
        auto derivedPtr = new VariantDerived<ValueType>(std::forward<ValueType>(value));
        auto basePtr = static_cast<VariantBase*>(derivedPtr);
        _value = std::unique_ptr<VariantBase>(basePtr);
        return *this;
    }

    template <typename ValueType>
    bool Variant::IsType() const
    {
        return (std::type_index(typeid(ValueType)) == _type);
    }

    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args&&... args)
    {
        auto derivedPtr = new VariantDerived<ValueType>(std::forward<Args>(args)...);
        auto basePtr = static_cast<VariantBase*>(derivedPtr);
        return Variant(std::type_index(typeid(ValueType)), std::unique_ptr<VariantBase>(basePtr));
    }

    //
    // Private code we'd like to hide:
    //
    namespace variantNamespace
    {
        template <typename ValueType>
        auto GetValueString(const ValueType& value, double) -> std::string
        {
            return std::string("[No to_string for type ") + typeid(ValueType).name() + "]";
        }

        using std::to_string;
        using utilities::to_string;
        inline std::string to_string(const std::string& str) { return str; }
        
        template <typename ValueType>
        auto GetValueString(const ValueType& value, int) -> decltype(to_string(value), std::string())
        {
            return to_string(value);
        }
    }

    template <typename ValueType>
    inline std::string VariantDerived<ValueType>::ToString() const
    {
        return variantNamespace::GetValueString(_value, (int)0);
    }

    template <typename ValueType>
    inline std::string VariantDerived<ValueType>::GetStoredTypeName() const
    {
        return TypeName<typename std::decay<ValueType>::type>::GetName();
    }
}
