////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchive.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchive.h"
#include "ObjectArchiver.h"
#include "IArchivable.h"

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
        return _properties;
    }

    ObjectArchive::PropertyCollection& ObjectArchive::GetProperties()
    {
        return _properties;
    }

    bool ObjectArchive::HasProperty(const std::string& name) const
    {
        return _properties.find(name) != _properties.end();
    }

    std::string ObjectArchive::GetValueString() const
    {
        if (HasValue())
        {
            return _value.ToString();
        }
        else
        {
            return "";
        }
    }

    const ObjectArchive& ObjectArchive::operator[](const std::string& propertyName) const
    {
        auto iter = _properties.find(propertyName);
        if (iter == _properties.end())
        {
            throw InputException(InputExceptionErrors::badData);
        }
        return iter->second;
    }

    ObjectArchive& ObjectArchive::operator[](const std::string& propertyName)
    {
        auto& prop = _properties[propertyName];
        return prop;
    }

    //
    // IArchivable
    //

    ObjectArchive IArchivable::GetDescription() const
    {
        utilities::SerializationContext context;
        utilities::ObjectArchiver archiver(context);
        archiver.Archive(*this);
        return archiver.GetObjectArchive();
    }
}
