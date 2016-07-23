////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace utilities
{
    template <typename T>
    void Serializer::Serialize(T&& obj)
    {
        auto desc = GetDescription(obj);
       SerializeType(desc.GetTypeName());
       for(const auto& item: desc)
       {
            SerializeField(item.first, item.second);
       }   
    }

//    template <typename ValueType>
//    ObjectDescription GetDescription(ValueType&& obj)
//    {
////        std::cout << "GetDescription(T&&)" << std::endl;
//        // TODO: use SFINAE fancyness to return obj.GetDescription if it exists, otherwise allow custom function
//        return obj.GetDescription();
//    }

    // TODO: a bunch of overloads here
    template<typename ValueType, typename std::enable_if_t<std::is_fundamental<ValueType>::value, int> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
        ObjectDescription description = ObjectDescription{obj};
        return description;
    }

    inline ObjectDescription GetDescription(const Variant& obj)
    {
//        std::cout << "GetDescription(Variant)" << std::endl;
//        std::cout << "\t" << obj.GetStoredTypeName() << std::endl;

        ObjectDescription result;
        obj.SetObjectDescription(result);
        return result;
    }
}
