////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ObjectArchive.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    //
    // ObjectArchive
    //
    template <typename ValueType>
    void ObjectArchive::SetType(const ValueType& object)
    {
        UNUSED(object);
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
    // Functions
    //
    template <typename ValueType>
    ValueType CreateObject(const ObjectArchive& archive)
    {
        return archive.GetValue<ValueType>();
    }
}
}
