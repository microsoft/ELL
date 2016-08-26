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
    ObjectDescription ObjectDescription::MakeObjectDescription(const std::string& description)
    {
        ObjectDescription result;
        result._description = description;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    void ObjectDescription::SetFillInDescriptionFunction(std::true_type)
    {
        _getPropertyDescription = [this]() 
        {
            auto ptr = this;
            auto typeDesc = ValueType::GetTypeDescription();
            return typeDesc;
        };
    }

    template <typename ValueType>
    void ObjectDescription::SetFillInDescriptionFunction(std::false_type)
    {
        _getPropertyDescription = nullptr;
    }

    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string description)
    {
        assert(_properties.find(name) == _properties.end());
        auto propertyDescription = ObjectDescription::MakeObjectDescription<ValueType>(description); // Here, add lambda to fill in description
        _properties[name] = std::move(propertyDescription);
        _properties[name].SetFillInDescriptionFunction<ValueType>(std::is_base_of<utilities::IDescribable, ValueType>());
    }

    template <typename ValueType>
    ValueType ObjectDescription::GetPropertyValue(const std::string& name) const
    {
        auto iter = _properties.find(name);
        if (iter == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        return iter->second.GetValue<ValueType>();
    }

    template <typename ValueType>
    void ObjectDescription::SetPropertyValue(const std::string& name, const ValueType& value)
    {
        auto iter = _properties.find(name);
        if (iter == _properties.end())
        {
            _properties[name] = MakeObjectDescription("", value);
        }
        else
        {
            iter->second.SetValue(value);
        }
    }

    template <typename ValueType>
    void ObjectDescription::SetValue(ValueType&& value)
    {
        // TODO: if valuetype is IDescribable, allow it to be a pointer (??)
        //        assert(_typeName == TypeName<typename std::decay<ValueType>::type>::GetName());
        _value = value;
    }

    template <typename ValueType>
    void ObjectDescription::operator=(ValueType&& value)
    {
        SetValue(value);
    }
}
