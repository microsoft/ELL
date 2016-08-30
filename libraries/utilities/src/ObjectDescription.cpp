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

    const ObjectDescription::PropertyCollection& ObjectDescription::Properties() const
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
        if(_getPropertiesFunction)
        {
            auto newProperties = _getPropertiesFunction();
            _properties = newProperties._properties;
            // _getPropertiesFunction = nullptr; // Note: can't nullify function here --- it needs to get called more than once (why?)
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
        // for each thing, serialize it
        const auto& description = GetDescription();
//                     Node::Serialize(serializer); // crap --- I think nodes will have to do custom serialization no matter what. :(
        for(const auto& property: description.Properties())
        {
            auto name = property.first;
            property.second._value.Serialize(name.c_str(), serializer);
        }
    }

    void IDescribable::Deserialize(Deserializer& serializer, SerializationContext& context)
    {
        auto description = GetDescription(); // will call GetDescription on a default-constructed object
        for(const auto& property: description.Properties())
        {
            auto name = property.first;
            auto variant = property.second._value;
//            variant.Deserialize(name.c_str(), serializer, context);
        }
        // crap... we need to set the value of the actual object. :( need to call the constructor somehow
    }

}
