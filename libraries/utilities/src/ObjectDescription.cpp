////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescription.h"

// utiliites
#include "Exception.h"

// stl
#include <memory>

namespace utilities
{
    //
    // ObjectDescription
    //
    ObjectDescription::ObjectDescription(const std::string& documentation) : _documentation(documentation) 
    {};

    const ObjectDescription::PropertyCollection& ObjectDescription::GetProperties() const
    {
        for(const auto& prop: _properties)
        {
            prop.second.FillInDescription();
        }
        return _properties;
    }

    ObjectDescription::PropertyCollection& ObjectDescription::GetProperties()
    {
        for(const auto& prop: _properties)
        {
            prop.second.FillInDescription();
        }
        return _properties;
    }

    bool ObjectDescription::HasProperty(const std::string& name) const
    {
        return _properties.find(name) != _properties.end();
    }

    std::string ObjectDescription::GetValueString() const
    {
        if(HasValue())
        {
            return _value.ToString();
        }
        else
        {
            return "";
        }
    }

    void ObjectDescription::FillInDescription() const 
    { 
        if(_fillInPropertiesFunction)
        {
            auto newProperties = _fillInPropertiesFunction(this);
            _properties = newProperties._properties;
            // _fillInPropertiesFunction = nullptr; // Note: can't nullify function here --- it needs to get called more than once (why?)
        }
    }

    const ObjectDescription& ObjectDescription::operator[](const std::string& propertyName) const
    {
        auto iter = _properties.find(propertyName); 
        if (iter == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        iter->second.FillInDescription();
        return iter->second;
    }

    ObjectDescription& ObjectDescription::operator[](const std::string& propertyName)
    {
        auto& prop = _properties[propertyName];        
        prop.FillInDescription();
        return prop;
    }

    //
    // IDescribable
    //

    void IDescribable::Serialize(Serializer& serializer) const
    {
        // for each property, serialize it
        const auto& description = GetDescription();
        for(const auto& property: description.GetProperties())
        {
            auto name = property.first;
            property.second._value.SerializeProperty(name.c_str(), serializer);
        }
    }

    void IDescribable::Deserialize(Deserializer& serializer)
    {
        // #### TODO: special-case vectors of pointers to return vectors of unique_ptrs

        auto description = GetDescription(); // will call GetDescription on a default-constructed object
        for(auto& property: description.GetProperties())
        {
            auto name = property.first;
            // std::cout << "Deserializing property " << name << std::endl;
            property.second._value.DeserializeProperty(name.c_str(), serializer, serializer.GetContext());
        }
        SetObjectState(description, serializer.GetContext());
    }
}
