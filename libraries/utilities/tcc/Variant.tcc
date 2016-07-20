////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    // Forward declaration of derived class
    template <typename ValueType>
    class VariantDerived;

    /// <summary> Base class for value holder </summary>
    class VariantBase
    {
    public:
        VariantBase(std::type_index type) : _type(type){};
        virtual ~VariantBase() = default;

        template <typename ValueType>
        ValueType GetValue() const
        {
            auto thisPtr = dynamic_cast<const VariantDerived<ValueType>*>(this);
            if (thisPtr == nullptr)
            {
                throw InputException(InputExceptionErrors::typeMismatch, "Variant::GetValue called with wrong type.");
            }

            return thisPtr->GetValue();
        }

        virtual std::unique_ptr<VariantBase> Clone() const = 0;

        virtual std::string ToString() const = 0;

        virtual std::string GetStoredTypeName() const = 0;

    private:
        std::type_index _type; // redundant with type in Variant class.
    };

    /// <summary>
    template <typename ValueType>
    class VariantDerived : public VariantBase
    {
    public:
        VariantDerived(const ValueType& val) : VariantBase(typeid(ValueType)), _value(val) {}
        const ValueType& GetValue() const { return _value; }

        virtual std::unique_ptr<VariantBase> Clone() const override
        {
            auto ptr = static_cast<VariantBase*>(new VariantDerived<ValueType>(_value));
            return std::unique_ptr<VariantBase>(ptr);
        }

        virtual std::string ToString() const override;

        virtual std::string GetStoredTypeName() const override;

    private:
        ValueType _value;
    };

    //
    // Private code we'd like to hide:
    //
    namespace VariantNamespace
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
        return VariantNamespace::GetValueString(_value, (int)0);
    }

    template <typename ValueType>
    inline std::string VariantDerived<ValueType>::GetStoredTypeName() const
    {
        return TypeName<ValueType>::GetName();
    }

    //
    // Variant
    //

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
}
