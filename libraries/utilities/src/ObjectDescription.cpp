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
            // _getPropertiesFunction = nullptr;
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
}
