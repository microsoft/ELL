////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ObjectArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchiver.h"
#include "Archiver.h"
#include "IArchivable.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace ell
{
namespace utilities
{
    ObjectArchiver::ObjectArchiver(SerializationContext context)
        : Unarchiver(context)
    {
    }

    ObjectArchiver::ObjectArchiver(const ObjectArchive& objectDescription, SerializationContext context)
        : Unarchiver(std::move(context)), _objectDescription(objectDescription)
    {
    }

    //
    // Serialization
    //
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, bool);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, char);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, short);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, int);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, size_t);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, uint64_t);
#endif
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, float);
    IMPLEMENT_ARCHIVE_VALUE(ObjectArchiver, double);

    // strings
    void ObjectArchiver::ArchiveValue(const char* name, const std::string& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription.SetValue(value);
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // IArchivable

    void ObjectArchiver::ArchiveObject(const char* name, const IArchivable& value)
    {
        if (std::string{ "" } == name)
        {
            Archiver::ArchiveObject(name, value);
        }
        else
        {
            // need to create new object description archiver here, I guess
            SerializationContext context;
            ObjectArchiver archiver(context);
            archiver << value;
            _objectDescription[name] = archiver.GetObjectArchive();
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, bool);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, char);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, short);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, int);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, size_t);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, uint64_t);
#endif
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, float);
    IMPLEMENT_ARCHIVE_ARRAY(ObjectArchiver, double);

    void ObjectArchiver::ArchiveArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void ObjectArchiver::ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription.SetValue(array);
            _objectDescription << array;
        }
        else
        {
            _objectDescription[name] << array;
        }
    }

    //
    // Deserialization
    //
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, bool);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, char);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, short);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, int);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, size_t);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, uint64_t);
#endif
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, float);
    IMPLEMENT_UNARCHIVE_VALUE(ObjectArchiver, double);

    // strings
    void ObjectArchiver::UnarchiveValue(const char* name, std::string& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> value;
        }
        else
        {
            _objectDescription[name] >> value;
        }
    }

    // IArchivable
    void ObjectArchiver::UnarchiveObject(const char* name, IArchivable& value)
    {
        if (std::string{ "" } == name)
        {
            Unarchiver::UnarchiveObject(name, value);
        }
        else
        {
            ObjectArchiver propertyUnarchiver(_objectDescription[name], GetContext());
            propertyUnarchiver.UnarchiveObject("", value);
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, bool);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, char);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, short);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, int);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, size_t);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, uint64_t);
#endif
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, float);
    IMPLEMENT_UNARCHIVE_ARRAY(ObjectArchiver, double);

    void ObjectArchiver::UnarchiveArray(const char* name, std::vector<std::string>& array)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> array;
        }
        else
        {
            _objectDescription[name] >> array;
        }
    }

    bool ObjectArchiver::BeginUnarchiveArrayItem(const std::string& typeName)
    {
        return true;
    }

    void ObjectArchiver::EndUnarchiveArrayItem(const std::string& typeName)
    {
    }
}
}
