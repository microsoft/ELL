////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchive.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // ObjectArchive
    //
    template <typename ValueType>
    void ObjectArchive::SetType(const ValueType& object)
    {
        _typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
    }

    template <typename ValueType>
    void ObjectArchive::SetGetPropertiesFunction(std::true_type)
    {
        // For some reason I don't understand, we need to pass in the pointer to this object
        // Somehow, the captured value of 'this' is incorrect
        _fillInPropertiesFunction = [](const ObjectArchive* self) 
        {
            if(self->HasValue())
            {
                ObjectArchive description;
                auto value = self->_value.GetValue<ValueType>();
                value.WriteToArchive(description);
                return description;
            }
            else
            {
                ObjectArchive description;
                typename std::decay<ValueType>::type value;
                value.WriteToArchive(description);
                return description;
            }
        };
    }

    template <typename ValueType>
    void ObjectArchive::SetGetPropertiesFunction(std::false_type)
    {
        _fillInPropertiesFunction = nullptr;
    }

    template <typename ValueType>
    void ObjectArchive::CopyValueTo(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectArchive::operator>>(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectArchive::SetValue(ValueType&& value)
    {
        SetType(value);
        _value = value;
        SetGetPropertiesFunction<typename std::decay<ValueType>::type>(std::is_base_of<utilities::IDescribable, typename std::decay<ValueType>::type>());
        FillInDescription();
    }

    template <typename ValueType>
    void ObjectArchive::operator<<(ValueType&& value)
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
