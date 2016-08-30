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
    ObjectDescription MakeObjectDescription(const std::string& documentation)
    {
        ObjectDescription result;
        result._documentation = documentation;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    void ObjectDescription::SetGetPropertiesFunction(std::true_type)
    {
        _getPropertiesFunction = [this]() 
        {
            if(HasValue())
            {
                auto ptr = _value.GetValue<ValueType>();
                return ptr.GetDescription();
            }
            else
            {
                return ValueType::GetTypeDescription();
            }
        };
    }

    template <typename ValueType>
    void ObjectDescription::SetGetPropertiesFunction(std::false_type)
    {
        _getPropertiesFunction = nullptr;
    }

    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string documentation)
    {
        assert(_properties.find(name) == _properties.end());
        _properties[name] = MakeObjectDescription<ValueType>(documentation);
        _properties[name].SetGetPropertiesFunction<ValueType>(std::is_base_of<utilities::IDescribable, ValueType>());
    }

    template <typename ValueType>
    void ObjectDescription::SetValue(ValueType&& value)
    {
        _value = value;
        FillInDescription();
    }

    template <typename ValueType>
    void ObjectDescription::operator=(ValueType&& value)
    {
        SetValue(value);
    }
}
