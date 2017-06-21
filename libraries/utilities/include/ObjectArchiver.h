////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ObjectArchiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "ObjectArchive.h"
#include "TypeFactory.h"
#include "TypeName.h"

// stl
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> An archiver that encodes data in an ObjectArchive </summary>
    class ObjectArchiver : public Archiver, public Unarchiver
    {
    public:
        /// <summary> Constructor for writing </summary>
        /// <param name="context"> The `SerializationContext` to use </param>
        ObjectArchiver(SerializationContext context);

        /// <summary> Constructor for reading </summary>
        ///
        /// <param name="objectDescription"> The description to read data from. </summary>
        /// <param name="context"> The `SerializationContext` to use </param>
        ObjectArchiver(const ObjectArchive& objectDescription, SerializationContext context);

        /// <summary> Gets the `ObjectArchive` containing the information for the archived object </summary>
        ///
        /// <returns> The `ObjectArchive` containing the information  for the archived object </returns>
        const ObjectArchive& GetObjectArchive() { return _objectDescription; }

    protected:
        // Serialization
        DECLARE_ARCHIVE_VALUE_OVERRIDE(bool);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(char);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(short);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(int);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(size_t);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(int64_t);
#ifdef __APPLE__
        DECLARE_ARCHIVE_VALUE_OVERRIDE(uint64_t);
#endif
        DECLARE_ARCHIVE_VALUE_OVERRIDE(float);
        DECLARE_ARCHIVE_VALUE_OVERRIDE(double);
        virtual void ArchiveValue(const char* name, const std::string& value) override;

        DECLARE_ARCHIVE_ARRAY_OVERRIDE(bool);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(char);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(short);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(int);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(size_t);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(int64_t);
#ifdef __APPLE__
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(uint64_t);
#endif
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(float);
        DECLARE_ARCHIVE_ARRAY_OVERRIDE(double);
        virtual void ArchiveArray(const char* name, const std::vector<std::string>& array) override;
        virtual void ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array) override;

        virtual void ArchiveObject(const char* name, const IArchivable& value) override;

        // Deserialization
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(bool);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(char);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(short);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(int);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(size_t);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(int64_t);
#ifdef __APPLE__
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(uint64_t);
#endif
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(float);
        DECLARE_UNARCHIVE_VALUE_OVERRIDE(double);
        virtual void UnarchiveValue(const char* name, std::string& value) override;

        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(bool);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(char);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(short);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(int);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(size_t);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(int64_t);
#ifdef __APPLE__
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(uint64_t);
#endif
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(float);
        DECLARE_UNARCHIVE_ARRAY_OVERRIDE(double);
        virtual void UnarchiveArray(const char* name, std::vector<std::string>& array) override;

        virtual void UnarchiveObject(const char* name, IArchivable& value) override;
        virtual bool BeginUnarchiveArrayItem(const std::string& typeName) override;
        virtual void EndUnarchiveArrayItem(const std::string& typeName) override;

    private:
        // Serialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        // Deserialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);

        void ReadArray(const char* name, std::vector<std::string>& array);

        // The object description
        ObjectArchive _objectDescription;
    };
}
}

#include "../tcc/ObjectArchiver.tcc"
