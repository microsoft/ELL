////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Archiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archiver.h"
#include "Exception.h"
#include "Format.h"
#include "IArchivable.h"
#include "Unused.h"

// stl
#include <string>

namespace ell
{
namespace utilities
{
    //
    // SerializationContext
    //

    GenericTypeFactory& SerializationContext::GetTypeFactory()
    {
        if (_previousContext != nullptr)
        {
            return _previousContext->GetTypeFactory();
        }
        return _typeFactory;
    }

    VariantTypeRegistry& SerializationContext::GetVariantTypeRegistry()
    {
        if (_previousContext != nullptr)
        {
            return _previousContext->GetVariantTypeRegistry();
        }
        return _variantTypeRegistry;
    }

    //
    // ArchivedObjectInfo
    //
    bool operator==(const ArchivedObjectInfo& a, const ArchivedObjectInfo& b)
    {
        return (a.type == b.type) && (a.version == b.version);
    }

    bool operator!=(const ArchivedObjectInfo& a, const ArchivedObjectInfo& b)
    {
        return !(a == b);
    }

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
        UNUSED(name, value);
        // nothing
    }

    void Archiver::ArchiveObject(const char* name, const IArchivable& value)
    {
        UNUSED(name);
        value.WriteToArchive(*this);
    }

    void Archiver::EndArchiveObject(const char* name, const IArchivable& value)
    {
        UNUSED(name, value);
        // nothing
    }

    ArchiveVersion Archiver::GetArchiveVersion(const IArchivable& value) const
    {
        return value.GetArchiveVersion();
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

    ArchivedObjectInfo Unarchiver::GetCurrentObjectInfo() const
    {
        return _objectInfo.back();
    }

    void Unarchiver::UnarchiveValue(const char* name, IArchivable& value)
    {
        auto objInfo = BeginUnarchiveObject(name, GetArchivedTypeName(value));
        _objectInfo.push_back(objInfo);

        if (!value.CanReadArchiveVersion(objInfo.version))
        {
            throw InputException(InputExceptionErrors::versionMismatch, "Attempting to read incompatible version");
        }
        auto typeName = objInfo.type;
        UnarchiveObject(name, value);
        EndUnarchiveObject(name, typeName);

        _objectInfo.pop_back();
    }

    ArchivedObjectInfo Unarchiver::BeginUnarchiveObject(const char* name, const std::string& typeName)
    {
        UNUSED(name);
        return { typeName, 0 };
    }

    void Unarchiver::UnarchiveObject(const char* name, IArchivable& value)
    {
        UNUSED(name);
        value.ReadFromArchive(*this);
    }

    void Unarchiver::UnarchiveObjectAsPrimitive(const char* name, IArchivable& value)
    {
        UNUSED(name);
        // ### Likely wrong
        value.ReadFromArchive(*this);
    }

    void Unarchiver::EndUnarchiveObject(const char* name, const std::string& typeName)
    {
        UNUSED(name, typeName);
    }

    void Unarchiver::BeginUnarchiveArray(const char* name, const std::string& typeName)
    {
        UNUSED(name, typeName);
    }

    void Unarchiver::EndUnarchiveArray(const char* name, const std::string& typeName)
    {
        UNUSED(name, typeName);
    }
}
}
