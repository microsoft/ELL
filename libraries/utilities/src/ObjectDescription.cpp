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

    void ObjectDescription::FillInDescription() 
    { 
        if(_getPropertiesFunction)
        {
            auto newProperties = _getPropertiesFunction(); 
        }
    }

    ObjectDescription& ObjectDescription::operator[](const std::string& propertyName)
    {
        FillInDescription();
        return _properties[propertyName];
    }
}
