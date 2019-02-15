////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IArchivable.h"
#include "StringUtil.h"
#include "Variant.h"

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace utilities
{
    /// <summary> PropertyBag: A metadata class consisting of a string -> Variant map </summary>
    class PropertyBag : public IArchivable
    {
    public:
        PropertyBag() = default;

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
        /// <returns> A const reference to the value for the given key, returned as a `Variant`. </returns>
        const Variant& GetEntry(const std::string& key) const;

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
        /// <param name="defaultValue"> The value to return if the key isn't present. </param>
        ///
        /// <returns> A const reference to the value for the given key. </returns>
        template <typename ValueType>
        const ValueType& GetEntry(const std::string& key, const ValueType& defaultValue) const;

        /// <summary> Get the typed metadata for a given key, parsing the value if it is stored as a string. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        ///
        /// <returns> A const reference to the value for the given key. </returns>
        template <typename ValueType>
        ValueType GetOrParseEntry(const std::string& key, std::function<ValueType(const std::string&)> parse = FromString<ValueType>) const;

        /// <summary> Get the typed metadata for a given key, parsing the value if it is stored as a string. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        /// <param name="defaultValue"> The value to return if the key isn't present. </param>
        ///
        /// <returns> A const reference to the value for the given key. </returns>
        template <typename ValueType>
        ValueType GetOrParseEntry(const std::string& key, const ValueType& defaultValue, std::function<ValueType(const std::string&)> parse = FromString<ValueType>) const;

        /// <summary> Find the given key and return its value (and add the key if it is not defined). </summary>
        ///
        /// <param name="key"> The key to add to the metadata. </param>
        ///
        /// <returns> A Variant reference you can update. </returns>
        Variant& operator[](const std::string& key);

        /// <summary> Removes metadata for the given key. </summary>
        ///
        /// <param name="key"> The key to use for the metadata. </param>
        ///
        /// <returns> The entry that was removed, or an empty `Variant` if none was present. </returns>
        Variant RemoveEntry(const std::string& key);

        /// <summary> Checks if there is a metadata entry for the given key. </summary>
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

        /// <summary> Gets the list of keys. </summary>
        ///
        /// <returns> The list of keys. </returns>
        std::vector<std::string> Keys() const;

        /// <summary> Returns an iterator to the first entry. </summary>
        auto begin() { return _metadata.begin(); }

        /// <summary> Returns an iterator to the first entry. </summary>
        auto begin() const { return _metadata.cbegin(); }

        /// <summary> Returns an iterator to a point just past the last entry. </summary>
        auto end() { return _metadata.end(); }

        /// <summary> Returns an iterator to a point just past the last entry. </summary>
        auto end() const { return _metadata.cend(); }

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
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    template <typename ValueType>
    void PropertyBag::SetEntry(const std::string& key, ValueType value)
    {
        _metadata[key] = Variant(value);
    }

    template <>
    inline void PropertyBag::SetEntry(const std::string& key, const char* value)
    {
        _metadata[key] = Variant(std::string(value));
    }

    template <typename ValueType>
    const ValueType& PropertyBag::GetEntry(const std::string& key) const
    {
        // This function throws an exception if there aren't any entries for the key, or if the underlying Variant doesn't contain a value of the correct type
        const auto& variant = _metadata.at(key);
        return variant.GetValue<ValueType>();
    }

    template <typename ValueType>
    const ValueType& PropertyBag::GetEntry(const std::string& key, const ValueType& defaultValue) const
    {
        if (!HasEntry(key))
        {
            return defaultValue;
        }
        return GetEntry<ValueType>(key);
    }

    template <typename ValueType>
    ValueType PropertyBag::GetOrParseEntry(const std::string& key, std::function<ValueType(const std::string&)> parse) const
    {
        const auto& variant = _metadata.at(key);
        if (variant.IsType<std::string>() && !std::is_same_v<ValueType, std::string>)
        {
            return parse(variant.GetValue<std::string>());
        }
        return variant.GetValue<ValueType>();
    }

    template <typename ValueType>
    ValueType PropertyBag::GetOrParseEntry(const std::string& key, const ValueType& defaultValue, std::function<ValueType(const std::string&)> parse) const
    {
        if (!HasEntry(key))
        {
            return defaultValue;
        }
        return GetOrParseEntry<ValueType>(key, parse);
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation
