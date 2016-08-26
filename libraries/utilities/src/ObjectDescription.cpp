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
    bool ObjectDescription::HasProperty(const std::string& name) const
    {
        return _properties.find(name) != _properties.end();
    }

    bool ObjectDescription::IsDescribable() const
    {
        return static_cast<bool>(_getPropertyDescription);
    }

    void ObjectDescription::FillInDescription() 
    { 
        if(_getPropertyDescription) 
        {
            auto newProperties = _getPropertyDescription(); 
            _properties = newProperties._properties;
            int x = 5;
        }
    }

    ObjectDescription& ObjectDescription::operator[](const std::string& propertyName)
    {
        return _properties[propertyName];
    }
}
