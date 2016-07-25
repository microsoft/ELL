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
    ObjectDescription ObjectDescription::FromType() 
    {
        auto result = ObjectDescription();
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    ObjectDescription ObjectDescription::FromType(ValueType&& obj)
    {
        auto result = ObjectDescription();
        result._typeName = TypeName<typename std::decay<ValueType>::type>::GetName();
        return result;
    }

    template <typename ValueType>
    void ObjectDescription::AddField(std::string name, ValueType&& value)
    {
        _description.insert(std::make_pair(name, MakeVariant<typename std::decay<ValueType>::type>(value))); 
    }
}
