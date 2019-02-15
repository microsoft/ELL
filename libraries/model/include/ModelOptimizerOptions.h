////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerOptions.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/PropertyBag.h>
#include <utilities/include/StringUtil.h>
#include <utilities/include/Variant.h>

#include <string>

namespace ell
{
namespace model
{
    enum class PreferredConvolutionMethod : int
    {
        automatic = 0,
        diagonal,
        simple,
        winograd,
        unrolled
    };

    // Interchange format:
    // when reconstituting from a general property bag, use strings for values
    // (or check type: allow either string or "real" type?)
    // maybe always use strings?

    class ModelOptimizerOptions
    {
    public:
        ModelOptimizerOptions() = default;
        explicit ModelOptimizerOptions(const utilities::PropertyBag& properties);

        /// <summary> Checks if there is a metadata entry for the given key. </summary>
        ///
        /// <param name="key"> The key to check for. </param>
        ///
        /// <returns> true if there is any metadata set for the given key. </returns>
        bool HasEntry(const std::string& key) const;

        /// <summary> Get the metadata for a given key. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        ///
        /// <returns> A const reference to the value for the given key, returned as a `Variant`. Throws an exception if the key isn't present. </returns>
        const utilities::Variant& GetEntry(const std::string& key) const;

        /// <summary> Get the metadata for a given key. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        ///
        /// <returns> A const reference to the value for the given key. Throws an exception if the key isn't present. </returns>
        template <typename ValueType>
        ValueType GetEntry(const std::string& key) const;

        /// <summary> Get the metadata for a given key. </summary>
        ///
        /// <param name="key"> The key for the metadata. </param>
        /// <param name="defaultValue"> The value to return if the key isn't present. </param>
        ///
        /// <returns> A const reference to the value for the given key. </returns>
        template <typename ValueType>
        ValueType GetEntry(const std::string& key, const ValueType& defaultValue) const;

        /// <summary> Find the given key and return its value (and add the key if it is not defined). </summary>
        ///
        /// <param name="key"> The key to add to the metadata. </param>
        ///
        /// <returns> A Variant reference you can update. </returns>
        utilities::Variant& operator[](const std::string& key);

        /// <summary> Adds a metadata entry for the given key. </summary>
        ///
        /// <param name="key"> The key to use for the metadata. </param>
        /// <param name="value"> The value to set for the given key. </param>
        template <typename ValueType>
        void SetEntry(const std::string& key, ValueType value);

        /// <summary> Returns the underlying `PropertyBag` object </summary>
        const utilities::PropertyBag& AsPropertyBag() const { return _options; }

        /// <summary> Returns the underlying `PropertyBag` object </summary>
        utilities::PropertyBag& AsPropertyBag() { return _options; }

        /// <summary> Returns an iterator to the first entry. </summary>
        auto begin() { return _options.begin(); }

        /// <summary> Returns a const iterator to the first entry. </summary>
        auto begin() const { return _options.begin(); }

        /// <summary> Returns an iterator to a point just past the last entry. </summary>
        auto end() { return _options.end(); }

        /// <summary> Returns a const iterator to a point just past the last entry. </summary>
        auto end() const { return _options.end(); }

    private:
        utilities::PropertyBag _options;
    };

    void AppendOptionsToMetadata(const ModelOptimizerOptions& options, utilities::PropertyBag& properties);
    void AppendMetadataToOptions(const utilities::PropertyBag& properties, ModelOptimizerOptions& options);

    std::string ToString(const PreferredConvolutionMethod& m);

} // namespace model

namespace utilities
{
    template <>
    model::PreferredConvolutionMethod FromString<model::PreferredConvolutionMethod>(const std::string& s);
}

} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename ValueType>
    ValueType ModelOptimizerOptions::GetEntry(const std::string& key) const
    {
        return _options.GetOrParseEntry<ValueType>(key);
    }

    template <typename ValueType>
    ValueType ModelOptimizerOptions::GetEntry(const std::string& key, const ValueType& defaultValue) const
    {
        return _options.GetOrParseEntry(key, defaultValue);
    }

    template <typename ValueType>
    void ModelOptimizerOptions::SetEntry(const std::string& key, ValueType value)
    {
        _options.SetEntry(key, value);
    }
} // namespace model
} // namespace ell
#pragma endregion implementation
