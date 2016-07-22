////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename ValueType>
    ObjectDescription::ObjectDescription(ValueType&& obj) : _typeName(TypeName<std::decay<ValueType>::type>::GetName())
    {}

    template <typename ValueType>
    void ObjectDescription::AddField(std::string name, ValueType&& value)
    {
        _description.insert(std::make_pair(name, MakeVariant<std::decay<ValueType>::type>(value))); 
    }
}
