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

    template <typename BaseType, typename ValueType>
    ObjectDescription MakeObjectDescription(const std::string& documentation)
    {
        ObjectDescription result = BaseType::GetTypeDescription();
        result._documentation = documentation;
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename BaseType, typename ValueType>
    ObjectDescription IDescribable::GetParentDescription() const
    {
        auto baseDescription = dynamic_cast<const BaseType*>(this)->BaseType::GetDescription();
        auto thisTypeDescription = ValueType::GetTypeDescription();

        // merge all properties values from base->this
        const auto& props = baseDescription.GetProperties();
        for (const auto& prop : props)
        {
            thisTypeDescription._properties[prop.first] = prop.second;
        }

        // value?
        // what about _fillInPropertiesFunction?? --- need to compose them (?)
        auto baseGetPropertiesFunction = baseDescription._fillInPropertiesFunction;
        auto newGetPropertiesFunction = thisTypeDescription._fillInPropertiesFunction;
        // For some reason I don't understand, we need to pass in the pointer to this object
        // Somehow, the captured value of 'this' is incorrect
        thisTypeDescription._fillInPropertiesFunction = [=](const ObjectDescription* self)
        {
            std::cout << "Combining functions" << std::endl;
            ObjectDescription baseProperties = baseGetPropertiesFunction(&baseDescription);
            ObjectDescription newProperties = newGetPropertiesFunction(&thisTypeDescription);
            for (const auto& prop : baseProperties.GetProperties())
            {
                newProperties._properties[prop.first] = prop.second;
            }
            return newProperties;
        };
        return thisTypeDescription;
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
                auto value = self->_value.GetValue<ValueType>();
                return value.GetDescription();
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
        _fillInPropertiesFunction = nullptr;
    }

    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string documentation)
    {
        assert(_properties.find(name) == _properties.end());
        _properties[name] = MakeObjectDescription<ValueType>(documentation);
        _properties[name].SetGetPropertiesFunction<ValueType>(std::is_base_of<utilities::IDescribable, ValueType>());
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
        _value = value;
        FillInDescription();
    }

    template <typename ValueType>
    void ObjectDescription::operator<<(ValueType&& value) const
    {
        SetValue(value);
    }

    template <typename ValueType>
    void ObjectDescription::operator=(ValueType&& value)
    {
        SetValue(value);
    }
}
