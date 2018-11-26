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
#include "TypeName.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> An archiver that encodes data in an ObjectArchive </summary>
    class ObjectArchiver : public Archiver
        , public Unarchiver
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

        /// <summary> Indicates if a property with the given name is available to be read next </summary>
        ///
        /// <param name="name"> The name of the property </param>
        ///
        /// <returns> true if a property with the given name can be read next </returns>
        bool HasNextPropertyName(const std::string& name) override;

    protected:
// Serialization
#define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void ArchiveValue(const char* name, const std::string& value) override;
        void ArchiveNull(const char* name) override;

#define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void ArchiveArray(const char* name, const std::vector<std::string>& array) override;
        void ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array) override;

        void ArchiveObject(const char* name, const IArchivable& value) override;

// Deserialization
#define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void UnarchiveValue(const char* name, std::string& value) override;
        bool UnarchiveNull(const char* name) override;

#define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void UnarchiveArray(const char* name, std::vector<std::string>& array) override;

        void UnarchiveObject(const char* name, IArchivable& value) override;
        bool BeginUnarchiveArrayItem(const std::string& typeName) override;
        void EndUnarchiveArrayItem(const std::string& typeName) override;

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
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::WriteScalar(const char* name, const ValueType& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // Specialization for bool (though perhaps this should be an overload, not a specialization)
    template <>
    inline void ObjectArchiver::WriteScalar(const char* name, const bool& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void ObjectArchiver::WriteScalar(const char* name, const char* value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << std::string{ value };
        }
        else
        {
            _objectDescription[name] << std::string{ value };
        }
    }

    inline void ObjectArchiver::WriteScalar(const char* name, const std::string& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    template <typename ValueType>
    void ObjectArchiver::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        if (std::string{ "" } == name)
        {
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
    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::ReadScalar(const char* name, ValueType& value)
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

    template <>
    inline void ObjectArchiver::ReadScalar(const char* name, bool& value)
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

    // This function is inline just so it appears next to the other Read* functions
    inline void ObjectArchiver::ReadScalar(const char* name, std::string& value)
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

    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::ReadArray(const char* name, std::vector<ValueType>& array)
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

    inline void ObjectArchiver::ReadArray(const char* name, std::vector<std::string>& array)
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
} // namespace utilities
} // namespace ell

#pragma endregion implementation
