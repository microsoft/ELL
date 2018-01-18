////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ObjectArchive.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchive.h"
#include "Exception.h"
#include "IArchivable.h"
#include "ObjectArchiver.h"

// stl
#include <memory>

namespace ell
{
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
        return *(iter->second);
    }

    ObjectArchive& ObjectArchive::operator[](const std::string& propertyName)
    {
        auto iter = _properties.find(propertyName);
        if (iter == _properties.end())
        {
            auto prop = std::make_shared<ObjectArchive>();
            _properties[propertyName] = prop;
            return *prop;
        }
        return *(iter->second);
    }
}
}
