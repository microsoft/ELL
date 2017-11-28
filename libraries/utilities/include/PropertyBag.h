////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IArchivable.h"
#include "Variant.h"

// stl
#include <string>
#include <unordered_map>

namespace ell
{
/// <summary> model namespace </summary>
namespace utilities
{
    /// <summary> PropertyBag: A metadata class consisting of a string -> Variant map </summary>
    class PropertyBag : public IArchivable
    {
    public:
        /// <summary> Adds a metadata entry for the given key. </summary>
        ///
        /// <param name="key"> The key to use for the metadata. </param>
        /// <param name="value"> The value to set for the given key. </param>
        template <typename ValueType>
        void SetEntry(const std::string& key, ValueType value);

        /// <summary> Get the metadata for a given key. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        ///
        /// <returns> A const reference to the value for the given key. </returns>
        template <typename ValueType>
        const ValueType& GetEntry(const std::string& key) const;

        /// <summary> Get the metadata for a given key. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        ///
        /// <returns> A Variant containing the value for the given key. </returns>
        Variant& operator[](const std::string& key);

        /// <summary> Removes metadata for the given key. </summary>
        ///
        /// <param name="key"> The key to use for the metadata. </param>
        Variant RemoveEntry(const std::string& key);

        /// <summary> Checks if is a metadata entries for the given key. </summary>
        ///
        /// <param name="key"> The key to check for. </param>
        ///
        /// <returns> true if there is any metadata set for the given key. </returns>
        bool HasEntry(const std::string& key) const;

        /// <summary> Checks if the property bag is empty. </summary>
        ///
        /// <returns> true if there are no entries in the property bag. </returns>
        bool IsEmpty() const;

        /// <summary> Removes all entries from the property bag. </summary>
        void Clear() { _metadata.clear(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PropertyBag"; }

    protected:
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        struct KeyValue : public IArchivable
        {
            std::string key;
            Variant value;

            KeyValue() = default;
            KeyValue(const std::string& key, const Variant& value);
            void WriteToArchive(Archiver& archiver) const override;
            void ReadFromArchive(Unarchiver& archiver) override;
            static std::string GetTypeName() { return "PropertyBag::KeyValue"; }
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }
        };

        void WriteToArchive(Archiver& archiver) const override;
        void ReadFromArchive(Unarchiver& archiver) override;

    private:
        std::unordered_map<std::string, Variant> _metadata;
    };
}
}

#include "../tcc/PropertyBag.tcc"
