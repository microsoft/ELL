////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Archiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archiver.h"
#include "Format.h"
#include "IArchivable.h"

#include <string>

namespace ell
{
namespace utilities
{
    //
    // PropertyArchiver class
    //
    Archiver::PropertyArchiver::PropertyArchiver(Archiver& archiver, const std::string& name)
        : _archiver(archiver), _propertyName(name){};

    //
    // Archiver class
    //
    Archiver::PropertyArchiver Archiver::operator[](const std::string& name)
    {
        return PropertyArchiver{ *this, name };
    }

    void Archiver::ArchiveValue(const char* name, const IArchivable& value)
    {
        BeginArchiveObject(name, value);
        ArchiveObject(name, value);
        EndArchiveObject(name, value);
    }

    void Archiver::BeginArchiveObject(const char* name, const IArchivable& value)
    {
        // nothing
    }

    void Archiver::ArchiveObject(const char* name, const IArchivable& value)
    {
        value.WriteToArchive(*this);
    }

    void Archiver::EndArchiveObject(const char* name, const IArchivable& value)
    {
        // nothing
    }

    //
    // PropertyArchiver class
    //
    Unarchiver::PropertyUnarchiver::PropertyUnarchiver(Unarchiver& archiver, const std::string& name)
        : _unarchiver(archiver), _propertyName(name){};

    //
    // Unarchiver class
    //
    Unarchiver::Unarchiver(SerializationContext context)
        : _baseContext(context)
    {
        _contexts.push_back(_baseContext);
    }

    Unarchiver::PropertyUnarchiver Unarchiver::operator[](const std::string& name)
    {
        return PropertyUnarchiver{ *this, name };
    }

    void Unarchiver::PushContext(SerializationContext& context)
    {
        _contexts.push_back(context);
    }

    void Unarchiver::UnarchiveValue(const char* name, IArchivable& value)
    {
        auto typeName = BeginUnarchiveObject(name, GetArchivedTypeName(value));
        UnarchiveObject(name, value);
        EndUnarchiveObject(name, typeName);
    }

    std::string Unarchiver::BeginUnarchiveObject(const char* name, const std::string& typeName)
    {
        return typeName;
    }

    void Unarchiver::UnarchiveObject(const char* name, IArchivable& value)
    {
        value.ReadFromArchive(*this);
    }

    void Unarchiver::UnarchiveObjectAsPrimitive(const char* name, IArchivable& value)
    {
        // ### Likely wrong
        value.ReadFromArchive(*this);
    }

    void Unarchiver::EndUnarchiveObject(const char* name, const std::string& typeName)
    {
    }

    void Unarchiver::BeginUnarchiveArray(const char* name, const std::string& typeName)
    {
    }

    void Unarchiver::EndUnarchiveArray(const char* name, const std::string& typeName)
    {
    }
}
}
