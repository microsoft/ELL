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
        VariantBase(std::type_index type): _type(type) {};
        virtual ~VariantBase() = default;

        template <typename ValueType>
        ValueType GetValue() const
        {
            auto thisPtr = dynamic_cast<const VariantDerived<ValueType>*>(this);
            if (thisPtr == nullptr)
            {
                throw std::runtime_error("Bad dynamic cast!");
            }

            return thisPtr->GetValue();
        }

        virtual std::unique_ptr<VariantBase> Clone() const = 0;

        //std::type_index GetType() const { return _type; }

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

        virtual std::unique_ptr<VariantBase> Clone() const
        {
            auto ptr = static_cast<VariantBase*>(new VariantDerived<ValueType>(_value));
            return std::unique_ptr<VariantBase>(ptr);
        }

    private:
        ValueType _value;
    };

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
    Variant MakeVariant(Args... args)
    {
        auto derivedPtr = new VariantDerived<ValueType>(args...);
        auto basePtr = static_cast<VariantBase*>(derivedPtr);
        return Variant(std::type_index(typeid(ValueType)), std::unique_ptr<VariantBase>(basePtr));
    }
}
