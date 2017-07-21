////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Archiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Exception.h"
#include "TypeFactory.h"
#include "TypeName.h"
#include "TypeTraits.h"

// stl
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    class IArchivable;
    class ArchivedAsPrimitive;

    /// <summary> Enabled if ValueType inherits from IArchivable. </summary>
    template <typename ValueType>
    using IsIArchivable = typename std::enable_if_t<std::is_base_of<IArchivable, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType inherits from IArchivable but not from ArchivedAsPrimitive. </summary>
    template <typename ValueType>
    using IsStandardArchivable = typename std::enable_if_t<std::is_base_of<IArchivable, typename std::decay<ValueType>::type>::value && !std::is_base_of<ArchivedAsPrimitive, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType inherits from IArchivable. </summary>
    template <typename ValueType>
    using IsArchivedAsPrimitive = typename std::enable_if_t<std::is_base_of<ArchivedAsPrimitive, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType does not inherit from ArchivedAsPrimitive. </summary>
    template <typename ValueType>
    using IsNotArchivedAsPrimitive = typename std::enable_if_t<!std::is_base_of<ArchivedAsPrimitive, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType does not inherit from IArchivable. </summary>
    template <typename ValueType>
    using IsNotArchivable = typename std::enable_if_t<(!std::is_base_of<IArchivable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;

    /// <summary> A context object used during deserialization. Contains a GenericTypeFactory. </summary>
    class SerializationContext
    {
    public:
        virtual ~SerializationContext() = default;

        /// <summary> Gets the type factory associated with this context. </summary>
        ///
        /// <returns> The type factory associated with this context. </returns>
        virtual GenericTypeFactory& GetTypeFactory() { return _typeFactory; }

    private:
        GenericTypeFactory _typeFactory;
    };

    /// <summary> Type to represent archive versions </summary>
    struct ArchiveVersion
    {
        int versionNumber; // 0 == None
    };

    bool operator==(const ArchiveVersion& a, const ArchiveVersion& b);
    bool operator!=(const ArchiveVersion& a, const ArchiveVersion& b);

    /// <summary> Info struct for archived objects </summary>
    struct ArchivedObjectInfo
    {
        std::string type;
        ArchiveVersion version;
    };

/// <summary> Macros to make repetitive boilerplate code in archiver implementations easier to implement. </summary>
#define DECLARE_ARCHIVE_VALUE_BASE(type) virtual void ArchiveValue(const char* name, type value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_ARCHIVE_ARRAY_BASE(type) virtual void ArchiveArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;

#define DECLARE_ARCHIVE_VALUE_OVERRIDE(type) virtual void ArchiveValue(const char* name, type value, IsFundamental<type> dummy = 0) override;
#define DECLARE_ARCHIVE_ARRAY_OVERRIDE(type) virtual void ArchiveArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    /// <summary>
    /// The Archiver and Unarchiver abstract base classes facilitate serialization and
    /// deserialization of some fundamental types, `std::string`s, `std::vector`s, and
    /// classes that implement the necessary functions. Serializing a couple of
    /// variables to a string stream is as simple as
    ///
    ///     double x = 5.3;
    ///     size_t y = 12;
    ///     std::stringstream stream;
    ///     ArchiverType archiver(stream);
    ///     archiver.WriteToArchive(x);
    ///     archiver.WriteToArchive(y);
    ///
    /// Deserialization must occur in the same order.
    ///
    ///     UnarchiverType unarchiver(stream);
    ///     double xx;
    ///     size_t yy;
    ///     unarchiver.deserialize(xx);
    ///     unarchiver.deserialize(yy);
    ///     assert(x == xx &amp;&amp; y == yy);
    ///
    /// The Archiver subclasses support serialization of named variables, in which case the
    /// deserialization must specify the correct variable name. This is most often used when
    /// serializing named fields in objects.
    ///
    ///     x = 0.4;
    ///     archiver.WriteToArchive("x", x);
    ///     unarchiver.deserialize("x", xx);
    ///     assert(x == xx);
    ///
    /// Serialization of `std::string`s and `std::vector`s of fundamental types is similar.
    ///
    /// To make a class archivable, the IArchivable class must be inherited from, and a default constructor
    /// needs to be implemented. Additionally, the static method `GetTypeName` needs to be implemented.
    ///
    ///     class Bar: public IArchivable
    ///     {
    ///     public:
    ///         Bar();
    ///         void ReadFromArchive(utilities::Unarchiver&amp; unarchiver) const
    ///         void WriteToArchive(utilities::Archiver&amp; archiver);
    ///         virtual std::string GetRuntimeTypeName() const;
    ///
    ///         static std::string GetTypeName();
    ///     }
    ///
    /// A typical implementation of Serialize will include a sequence of
    /// calls to archiver.WriteToArchive(), in the same order. To serialize the class, call:
    ///
    ///     Bar z;
    ///     archiver.WriteToArchive("z", z);
    ///
    /// A typical implementation of ReadFromArchive() will include a similar sequence of calls to
    /// archiver.ReadFromArchive().
    ///
    /// Serialization and deserialization of pointers and `std::unique_pointer`s to archivable objects
    /// (that is, classes that derive from IArchivable, have a default constructor, and implement
    /// the static `GetTypeName` function) is supported as well.
    ///
    /// </summary>
    class Archiver
    {
    public:
        /// <summary> Represents an archiver that is scoped to a particular property. </summary>
        class PropertyArchiver
        {
        public:
            /// <summary> Write the property to the archiver. </summary>
            ///
            /// <typeparam name="ValueType"> The type of the property. </typeparam>
            /// <param name="value"> The property to serialize. </param>
            template <typename ValueType>
            void operator<<(ValueType&& value);

        private:
            friend class Archiver;
            PropertyArchiver(Archiver& archiver, const std::string& name);
            Archiver& _archiver;
            std::string _propertyName;
        };

        /// <summary> Destructor </summary>
        virtual ~Archiver() { EndArchiving(); }

        /// <summary> Add value to an archive. </summary>
        ///
        /// <param name="value"> The value to add to the archive. </param>
        template <typename ValueType>
        void Archive(ValueType&& value);

        /// <summary> Add value to an archive. </summary>
        ///
        /// <param name="value"> The value to add to the archive. </param>
        template <typename ValueType>
        void operator<<(ValueType&& value);

        /// <summary> Add named value to an archive. </summary>
        ///
        /// <param name="name"> The name to archive the value under. </param>
        /// <param name="value"> The value to add to the archive. </param>
        template <typename ValueType>
        void Archive(const char* name, ValueType&& value);

        /// <summary> Get an archiver scoped to a particular property name. </summary>
        ///
        /// <param name="name"> The name to archive the value under. </param>
        /// <returns> An archiver that will save an object with the given name </returns>
        PropertyArchiver operator[](const std::string& name);

    protected:
        // These are all the virtual function that need to be implemented by archivers
        DECLARE_ARCHIVE_VALUE_BASE(bool);
        DECLARE_ARCHIVE_VALUE_BASE(char);
        DECLARE_ARCHIVE_VALUE_BASE(short);
        DECLARE_ARCHIVE_VALUE_BASE(int);
        DECLARE_ARCHIVE_VALUE_BASE(size_t);
        DECLARE_ARCHIVE_VALUE_BASE(int64_t);
#ifdef __APPLE__
        DECLARE_ARCHIVE_VALUE_BASE(uint64_t);
#endif
        DECLARE_ARCHIVE_VALUE_BASE(float);
        DECLARE_ARCHIVE_VALUE_BASE(double);
        virtual void ArchiveValue(const char* name, const std::string& value) = 0;
        virtual void ArchiveValue(const char* name, const IArchivable& value);

        DECLARE_ARCHIVE_ARRAY_BASE(bool);
        DECLARE_ARCHIVE_ARRAY_BASE(char);
        DECLARE_ARCHIVE_ARRAY_BASE(short);
        DECLARE_ARCHIVE_ARRAY_BASE(int);
        DECLARE_ARCHIVE_ARRAY_BASE(size_t);
        DECLARE_ARCHIVE_ARRAY_BASE(int64_t);
#ifdef __APPLE__
        DECLARE_ARCHIVE_ARRAY_BASE(uint64_t);
#endif
        DECLARE_ARCHIVE_ARRAY_BASE(float);
        DECLARE_ARCHIVE_ARRAY_BASE(double);
        virtual void ArchiveArray(const char* name, const std::vector<std::string>& array) = 0;
        virtual void ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array) = 0;

        virtual void BeginArchiveObject(const char* name, const IArchivable& value);
        virtual void ArchiveObject(const char* name, const IArchivable& value);
        virtual void EndArchiveObject(const char* name, const IArchivable& value);

        virtual void EndArchiving() {}

        ArchiveVersion GetArchiveVersion(const IArchivable& value) const;

    private:
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void ArchiveItem(const char* name, ValueType&& value);

        template <typename ValueType>
        void ArchiveItem(const char* name, ValueType* value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ArchiveItem(const char* name, const std::vector<ValueType>& value);

        void ArchiveItem(const char* name, const std::vector<std::string>& value);

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void ArchiveItem(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void ArchiveItem(const char* name, const std::vector<const ValueType*>& value);
    };

/// <summary> Macros to make repetitive boilerplate code in unarchiver implementations easier to implement. </summary>
#define DECLARE_UNARCHIVE_VALUE_BASE(type) virtual void UnarchiveValue(const char* name, type& value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_UNARCHIVE_ARRAY_BASE(type) virtual void UnarchiveArray(const char* name, std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_UNARCHIVE_VALUE_OVERRIDE(type) virtual void UnarchiveValue(const char* name, type& value, IsFundamental<type> dummy = 0) override;
#define DECLARE_UNARCHIVE_ARRAY_OVERRIDE(type) virtual void UnarchiveArray(const char* name, std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    /// <summary> Unarchiver class </summary>
    class Unarchiver
    {
    public:
        /// <summary> Represents an unarchiver that is scoped to a particular property. </summary>
        class PropertyUnarchiver
        {
        public:
            /// <summary> Reads the property from the archive. </summary>
            ///
            /// <typeparam name="ValueType"> The type of the property. </typeparam>
            /// <param name="value"> The variable to read the property into. </param>
            template <typename ValueType>
            void operator>>(ValueType&& value);

        private:
            friend class Unarchiver;
            PropertyUnarchiver(Unarchiver& unarchiver, const std::string& name);
            Unarchiver& _unarchiver;
            std::string _propertyName;
        };

        /// <summary> Constructor </summary>
        ///
        /// <param name="context"> The initial `SerializationContext` to use </param>
        Unarchiver(SerializationContext context);

        /// <summary> Destructor </summary>
        virtual ~Unarchiver() { EndUnarchiving(); }

        /// <summary> Read value from an archive. </summary>
        ///
        /// <param name="value"> The value to read into. </param>
        template <typename ValueType>
        void Unarchive(ValueType&& value);

        /// <summary> Read value from an archive. </summary>
        ///
        /// <param name="value"> The value to read into. </param>
        template <typename ValueType>
        void operator>>(ValueType&& value);

        /// <summary> Read named value from an archive. </summary>
        ///
        /// <param name="name"> The name of the value to read. </param>
        /// <param name="value"> The value to read. </param>
        template <typename ValueType>
        void Unarchive(const char* name, ValueType&& value);

        /// <summary> Get an unarchiver scoped to a particular property name. </summary>
        ///
        /// <param name="name"> The name of the property </param>
        PropertyUnarchiver operator[](const std::string& name);

        /// <summary> Set a new serialization context to be current </summary>
        ///
        /// <param name="context"> The context </param>
        void PushContext(SerializationContext& context);

        /// <summary> Remove the current serialization context and use the previous one </summary>
        void PopContext() { _contexts.pop_back(); }

        /// <summary> Get the current serialization context </summary>
        ///
        /// <returns> The current serialization context </returns>
        SerializationContext& GetContext() { return _contexts.back(); }

    protected:
        DECLARE_UNARCHIVE_VALUE_BASE(bool);
        DECLARE_UNARCHIVE_VALUE_BASE(char);
        DECLARE_UNARCHIVE_VALUE_BASE(short);
        DECLARE_UNARCHIVE_VALUE_BASE(int);
        DECLARE_UNARCHIVE_VALUE_BASE(size_t);
        DECLARE_UNARCHIVE_VALUE_BASE(int64_t);
#ifdef __APPLE__
        DECLARE_UNARCHIVE_VALUE_BASE(uint64_t);
#endif
        DECLARE_UNARCHIVE_VALUE_BASE(float);
        DECLARE_UNARCHIVE_VALUE_BASE(double);
        virtual void UnarchiveValue(const char* name, std::string& value) = 0;
        virtual void UnarchiveValue(const char* name, IArchivable& value);

        DECLARE_UNARCHIVE_ARRAY_BASE(bool);
        DECLARE_UNARCHIVE_ARRAY_BASE(char);
        DECLARE_UNARCHIVE_ARRAY_BASE(short);
        DECLARE_UNARCHIVE_ARRAY_BASE(int);
        DECLARE_UNARCHIVE_ARRAY_BASE(size_t);
        DECLARE_UNARCHIVE_ARRAY_BASE(int64_t);
#ifdef __APPLE__
        DECLARE_UNARCHIVE_ARRAY_BASE(uint64_t);
#endif
        DECLARE_UNARCHIVE_ARRAY_BASE(float);
        DECLARE_UNARCHIVE_ARRAY_BASE(double);
        virtual void UnarchiveArray(const char* name, std::vector<std::string>& array) = 0;

        // Extra functions needed for deserializing arrays.
        virtual void BeginUnarchiveArray(const char* name, const std::string& typeName);
        virtual bool BeginUnarchiveArrayItem(const std::string& typeName) = 0;
        virtual void EndUnarchiveArrayItem(const std::string& typeName) = 0;
        virtual void EndUnarchiveArray(const char* name, const std::string& typeName);

        // Extra functions needed for deserializing IArchivable objects.
        virtual ArchivedObjectInfo BeginUnarchiveObject(const char* name, const std::string& typeName);
        virtual void UnarchiveObject(const char* name, IArchivable& value);
        virtual void EndUnarchiveObject(const char* name, const std::string& typeName);
        virtual void UnarchiveObjectAsPrimitive(const char* name, IArchivable& value);

        virtual void EndUnarchiving() {}

    private:
        SerializationContext _baseContext;
        std::vector<std::reference_wrapper<SerializationContext>> _contexts;

        // non-vector standard thing
        template <typename ValueType, IsNotVector<ValueType> concept1 = 0, IsNotArchivedAsPrimitive<ValueType> concept2 = 0>
        void UnarchiveItem(const char* name, ValueType&& value);

        // non-vector archivable-as-fundamental
        template <typename ValueType, IsNotVector<ValueType> concept1 = 0, IsArchivedAsPrimitive<ValueType> concept2 = 0>
        void UnarchiveItem(const char* name, ValueType&& value);

        // unique pointer to non-archivable object
        template <typename ValueType, IsNotArchivable<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value);

        // unique pointer to standard archivable object
        template <typename ValueType, IsStandardArchivable<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value);

        // unique pointer to archived-as-primitive object
        template <typename ValueType, IsArchivedAsPrimitive<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value);

        // vector of fundamental values
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::vector<ValueType>& value);

        // vector of strings
        void UnarchiveItem(const char* name, std::vector<std::string>& value);

        // vector of IArchivable values
        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::vector<ValueType>& value);

        // vector of unique pointers to IArchivable
        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::vector<std::unique_ptr<ValueType>>& value);

        // vector of pointers to IArchivable
        template <typename ValueType, IsIArchivable<ValueType> concept = 0>
        void UnarchiveItem(const char* name, std::vector<const ValueType*>& value);
    };

    //
    // Utility functions useful to archivers and unarchivers
    //

    /// <summary> Returns type name used for serialization </summary>
    ///
    /// <typeparam name="T"> The type to serialize </typeparam>
    /// <returns> The name to use when serializing type `T` </returns>
    template <typename T>
    std::string GetArchivedTypeName();

    /// <summary> Returns type name used for serialization of a (potentially-)polymorphic object </summary>
    ///
    /// <param name="value"> The value to serialize </param>
    /// <returns> The name to use when serializing the value </returns>
    template <typename T>
    std::string GetArchivedTypeName(const T& value);

/// <summary> Macros to make repetitive boilerplate code in archiver implementations easier to implement. </summary>
#define IMPLEMENT_ARCHIVE_VALUE(base, type) \
    void base::ArchiveValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name, value); }
#define IMPLEMENT_ARCHIVE_ARRAY(base, type) \
    void base::ArchiveArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name, value); }

/// <summary> Macros to make repetitive boilerplate code in unarchiver implementations easier to implement. </summary>
#define IMPLEMENT_UNARCHIVE_VALUE(base, type) \
    void base::UnarchiveValue(const char* name, type& value, IsFundamental<type> dummy) { ReadScalar(name, value); }
#define IMPLEMENT_UNARCHIVE_ARRAY(base, type) \
    void base::UnarchiveArray(const char* name, std::vector<type>& value, IsFundamental<type> dummy) { ReadArray(name, value); }
}
}

#include "../tcc/Archiver.tcc"
