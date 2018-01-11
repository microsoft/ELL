////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename ValueType>
    void PropertyBag::SetEntry(const std::string& key, ValueType value)
    {
        _metadata[key] = Variant(value);
    }

    template <typename ValueType>
    const ValueType& PropertyBag::GetEntry(const std::string& key) const
    {
        // This function throws an exception if there aren't any entries for the key, or if the underlying Variant doesn't contain a value of the correct type
        const auto& variant = _metadata.at(key);
        return variant.GetValue<ValueType>();
    }
}
}
