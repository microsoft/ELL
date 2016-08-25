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
    // PropertyDescription
    //
    template <typename ValueType>
    PropertyDescription PropertyDescription::MakePropertyDescription(const std::string& description)
    {
        PropertyDescription result;
        result._description = description;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    PropertyDescription PropertyDescription::MakePropertyDescription(const std::string& description, const ValueType& value)
    {
        PropertyDescription result;
        result._description = description;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        result._value = MakeVariant<typename std::decay<ValueType>::type>(value);
        return result;
    }

    template <typename ValueType>
    void PropertyDescription::SetValue(ValueType&& value)
    {
        // TODO: if valuetype is IDescribable, allow it to be a pointer (??)
//        assert(_typeName == TypeName<typename std::decay<ValueType>::type>::GetName());
        _value = value;
    }

    template <typename ValueType>
    void PropertyDescription::operator=(ValueType&& value)
    {
        SetValue(value);
    }

    //
    // ObjectDescription
    //
    template <typename ValueType>
    ObjectDescription ObjectDescription::MakeObjectDescription(const std::string& description)
    {
        ObjectDescription result;
        result._description = description;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string description)
    {
        assert(_properties.find(name) == _properties.end());
        _properties[name] = PropertyDescription::MakePropertyDescription<ValueType>(description);
    }

    template <typename ValueType>
    ValueType ObjectDescription::GetPropertyValue(const std::string& name) const
    {
        auto iter = _properties.find(name); 
        if(iter == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        return iter->second.GetValue<ValueType>();
    }

    template <typename ValueType>
    void ObjectDescription::SetPropertyValue(const std::string& name, const ValueType& value)
    {
        auto iter = _properties.find(name); 
        if(iter == _properties.end())
        {
            _properties[name] = PropertyDescription("", value);
        }
        else
        {
            iter->second.SetValue(value);
        }
    }
}
