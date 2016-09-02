////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // ObjectDescription
    //
    template <typename ValueType>
    void ObjectDescription::SetType(const ValueType& object)
    {
        _typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
    }

    template <typename ValueType>
    void ObjectDescription::SetGetPropertiesFunction(std::true_type)
    {
        // For some reason I don't understand, we need to pass in the pointer to this object
        // Somehow, the captured value of 'this' is incorrect
        _fillInPropertiesFunction = [](const ObjectDescription* self) 
        {
            if(self->HasValue())
            {
                ObjectDescription description;
                auto value = self->_value.GetValue<ValueType>();
                value.Serialize(description);
                return description;
            }
            else
            {
                ObjectDescription description;
                typename std::decay<ValueType>::type value;
                value.Serialize(description);
                return description;
            }
        };
    }

    template <typename ValueType>
    void ObjectDescription::SetGetPropertiesFunction(std::false_type)
    {
        _fillInPropertiesFunction = nullptr;
    }

    template <typename ValueType>
    void ObjectDescription::CopyValueTo(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectDescription::operator>>(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectDescription::SetValue(ValueType&& value)
    {
        SetType(value);
        _value = value;
        SetGetPropertiesFunction<typename std::decay<ValueType>::type>(std::is_base_of<utilities::IDescribable, typename std::decay<ValueType>::type>());
        FillInDescription();
    }

    template <typename ValueType>
    void ObjectDescription::operator<<(ValueType&& value)
    {
        SetValue(value);
    }

    //
    // IDescribable
    //
    template <typename ValueType>
    static ValueType CreateObject(const Archiver& archiver)
    {
        // TODO: force archiver to "deserialize" if necessary (?)
        return archiver.GetValue<ValueType>();
    }

}
