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
    PropertyDescription::PropertyDescription(const std::string& description, const ValueType& value)
    {
        _description = description;
        _value = MakeVariant<ValueType>(value);
    }

    //
    // ObjectDescription
    //
    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string description)
    {
        ValueType dummyValue;
        AddProperty(name, description, dummyValue);
    }

    template <typename ValueType>
    void ObjectDescription::AddProperty(const std::string& name, std::string description, const ValueType& value)
    {
        assert(_properties.find(name) == _properties.end());
        _properties[name] = PropertyDescription(description, value);
    }

    template <typename ValueType>
    ValueType ObjectDescription::GetPropertyValue(const std::string& name)
    {
        if(_properties.find(name) == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        auto property = _properties[name];
        return property.GetValue<ValueType>();
    }
}
