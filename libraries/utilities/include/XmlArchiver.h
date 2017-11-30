////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     XmlArchiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "Tokenizer.h"
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
    /// <summary> An archiver that encodes data in an XML format. </summary>
    class XmlArchiver : public Archiver
    {
    public:
        /// <summary> Default Constructor --- writes to standard output. </summary>
        XmlArchiver();

        /// <summary> Constructor </summary>
        ///
        /// <param name="outputStream"> The stream to write data to. </summary>
        XmlArchiver(std::ostream& outputStream);

    protected:
        #define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
        #undef ARCHIVE_TYPE_OP
        void ArchiveValue(const char* name, const std::string& value) override;

        #define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
        #undef ARCHIVE_TYPE_OP

        void ArchiveArray(const char* name, const std::vector<std::string>& array) override;
        void ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array) override;

        void BeginArchiveObject(const char* name, const IArchivable& value) override;
        void EndArchiveObject(const char* name, const IArchivable& value) override;

        void EndArchiving() override;

    private:
        // Serialization
        void WriteFileHeader();
        void WriteFileFooter();

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        void CloseStream();

        std::string GetCurrentIndent() { return std::string(2 * _indent, ' '); }
        void IncrementIndent() { ++_indent; }
        void DecrementIndent() { --_indent; }
        void SetIndent(int indent);

        std::ostream& _out;
        bool _ready = false;
        int _indent = 0;
    };

    /// <summary> An unarchiver that reads data encoded in an XML format. </summary>
    class XmlUnarchiver : public Unarchiver
    {
    public:
        /// <summary> Default Constructor --- reads from standard input. </summary>
        XmlUnarchiver(SerializationContext context);

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputStream"> The stream to read data from. </summary>
        XmlUnarchiver(std::istream& inputStream, SerializationContext context);

        /// <summary> Indicates if a property with the given name is available to be read next </summary>
        ///
        /// <param name="name"> The name of the property </param>
        ///
        /// <returns> true if a property with the given name can be read next </returns>
        bool HasNextPropertyName(const std::string& name) override;

    protected:
        #define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
        #undef ARCHIVE_TYPE_OP

        void UnarchiveValue(const char* name, std::string& value) override;

        #define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
        #undef ARCHIVE_TYPE_OP

        void UnarchiveArray(const char* name, std::vector<std::string>& array) override;
        void BeginUnarchiveArray(const char* name, const std::string& typeName) override;
        bool BeginUnarchiveArrayItem(const std::string& typeName) override;
        void EndUnarchiveArrayItem(const std::string& typeName) override;
        void EndUnarchiveArray(const char* name, const std::string& typeName) override;

        ArchivedObjectInfo BeginUnarchiveObject(const char* name, const std::string& typeName) override;
        void EndUnarchiveObject(const char* name, const std::string& typeName) override;

        void EndUnarchiving() override;

    private:
        // Deserialization
        void ReadFileHeader();
        void ReadFileFooter();

        template <typename ValueType, IsIntegral<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);
        template <typename ValueType, IsFloatingPoint<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);
        void ReadScalar(const char* name, std::string& value);
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);
        void ReadArray(const char* name, std::vector<std::string>& array);

        Tokenizer _tokenizer;
    };

    /// <summary> XML utility functions --- for internal use by `XMLArchiver` and `XMLUnarchiver` </summary>
    class XmlUtilities
    {
    public:
        static std::string EncodeAttributeString(const std::string& str);
        static std::string DecodeAttributeString(const std::string& str);
        static std::string EncodeTypeName(const std::string& str);
        static std::string DecodeTypeName(const std::string& str);
    };
}
}

#include "../tcc/XmlArchiver.tcc"
