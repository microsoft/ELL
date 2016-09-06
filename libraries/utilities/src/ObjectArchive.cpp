////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchive.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchive.h"
#include "ObjectArchiver.h"

// utiliites
#include "Exception.h"

// stl
#include <memory>

namespace utilities
{
    //
    // ObjectArchive
    //
    const ObjectArchive::PropertyCollection& ObjectArchive::GetProperties() const
    {
        for(const auto& prop: _properties)
        {
            prop.second.FillInDescription();
        }
        return _properties;
    }

    ObjectArchive::PropertyCollection& ObjectArchive::GetProperties()
    {
        for(const auto& prop: _properties)
        {
            prop.second.FillInDescription();
        }
        return _properties;
    }

    bool ObjectArchive::HasProperty(const std::string& name) const
    {
        return _properties.find(name) != _properties.end();
    }

    std::string ObjectArchive::GetValueString() const
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

    void ObjectArchive::FillInDescription() const 
    { 
        if(_fillInPropertiesFunction)
        {
            auto newProperties = _fillInPropertiesFunction(this);
            _properties = newProperties._properties;
            // _fillInPropertiesFunction = nullptr; // Note: can't nullify function here --- it needs to get called more than once (why?)
        }
    }

    const ObjectArchive& ObjectArchive::operator[](const std::string& propertyName) const
    {
        auto iter = _properties.find(propertyName); 
        if (iter == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        iter->second.FillInDescription();
        return iter->second;
    }

    ObjectArchive& ObjectArchive::operator[](const std::string& propertyName)
    {
        auto& prop = _properties[propertyName];
        return prop;
    }

    //
    // IDescribable
    //

    ObjectArchive IDescribable::GetDescription() const
    {
        utilities::SerializationContext context;
        utilities::ObjectArchiver archiver(context);
        archiver.Serialize(*this);
        return archiver.GetObjectArchive();
    }
}
