////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     JsonArchiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "Tokenizer.h"

// stl
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> An archiver that encodes data as JSON-formatted text </summary>
    class JsonArchiver : public Archiver
    {
    public:
        /// <summary> Default Constructor --- writes to standard output. </summary>
        JsonArchiver();

        /// <summary> Constructor </summary>
        ///
        /// <param name="outputStream"> The stream to write data to. </param>
        JsonArchiver(std::ostream& outputStream);

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
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        // Utility functions
        void WriteObjectType(const IArchivable& value);
        void WriteObjectVersion(const IArchivable& value);
        void IncrementIndent() { ++_indent; }
        void DecrementIndent() { --_indent; }
        std::string GetCurrentIndent() { return std::string(2 * _indent, ' '); }
        void Indent();
        void FinishPreviousLine();
        void SetEndOfLine(std::string endOfLine);

        std::ostream& _out;
        int _indent = 0;
        std::string _endOfPreviousLine;
    };

    /// <summary> An unarchiver that reads data encoded in JSON-formatted text. </summary>
    class JsonUnarchiver : public Unarchiver
    {
    public:
        /// <summary> Default Constructor --- reads from standard input. </summary>
        JsonUnarchiver(SerializationContext context);

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputStream"> The stream to read data from. </summary>
        JsonUnarchiver(std::istream& inputStream, SerializationContext context);

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
        void UnarchiveObjectAsPrimitive(const char* name, IArchivable& value) override;

    private:
        template <typename ValueType, IsIntegral<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        template <typename ValueType, IsFloatingPoint<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);

        void ReadArray(const char* name, std::vector<std::string>& array);

        bool TryMatchFieldName(const char* name, std::string& found);
        void MatchFieldName(const char* name);

        std::string _endOfPreviousLine;
        Tokenizer _tokenizer;
    };

    // Json utility functions
    class JsonUtilities
    {
    public:
        /// <summary></summary>
        static std::string EncodeString(const std::string& str);

        /// <summary></summary>
        static std::string DecodeString(const std::string& str);

        /// <summary></summary>
        static std::string EncodeTypeName(const std::string& str);

        /// <summary></summary>
        static std::string DecodeTypeName(const std::string& str);
    };
}
}

#include "../tcc/JsonArchiver.tcc"
