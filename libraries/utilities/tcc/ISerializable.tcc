////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename ValueType>
    ObjectDescription::ObjectDescription(ValueType&& obj) : _typeName(TypeName<typename std::decay<ValueType>::type>::GetName())
    {}

    template <typename T>
    ObjectDescription GetDescription(T&& obj)
    {
        // TODO: use SFINAE fancyness to return obj.GetDescription if it exists, otherwise allow custom function
        return obj.GetDescription();
    }

    template <typename T>
    void Serializer::Serialize(T&& obj)
    {
       auto desc = obj.GetDescription();
       SerializeType(obj.GetTypeName());
       for(const auto& item: desc)
       {
            SerializeField(item.first, item.second);
       }   
    }
}
