////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchive.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // ObjectArchive
    //
    template <typename ValueType>
    void ObjectArchive::SetType(const ValueType& object)
    {
        _typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
    }

    template <typename ValueType>
    void ObjectArchive::CopyValueTo(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectArchive::operator>>(ValueType&& value) const
    {
        value = _value.GetValue<typename std::decay<ValueType>::type>();
    }

    template <typename ValueType>
    void ObjectArchive::SetValue(ValueType&& value)
    {
        SetType(value);
        _value = value;
    }

    template <typename ValueType>
    void ObjectArchive::operator<<(ValueType&& value)
    {
        SetValue(value);
    }

    //
    // IDescribable
    //
    template <typename ValueType>
    static ValueType CreateObject(const Archiver& archiver)
    {
        // TODO: force archiver to "deserialize" if necessary (?)
        return archiver.GetValue<ValueType>();
    }
}
