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
    // helper function
    template <typename ValueType> // , typename std::enable_if_t<!std::is_fundamental<ValueType>::value, int> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
        std::cout << "GetDescription(T&&)" << std::endl;
        // TODO: use SFINAE fancyness to return obj.GetDescription if it exists, otherwise allow a free function
        return obj.GetDescription();
    }

    // TODO: a bunch of overloads here
    template <typename ValueType, typename std::enable_if_t<std::is_fundamental<ValueType>::value, int> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
        std::cout << "GetDescription(fundamental)" << std::endl;
        ObjectDescription description = ObjectDescription{ obj };
        return description;
    }

    inline ObjectDescription GetDescription(const Variant& obj)
    {
        std::cout << "GetDescription(Variant)" << std::endl;
        //        std::cout << "GetDescription(Variant)" << std::endl;
        //        std::cout << "\t" << obj.GetStoredTypeName() << std::endl;

        auto result = *(obj.GetObjectDescription());
        return result;
    }

    template <typename T>
    void Serializer::Serialize(T&& obj)
    {
        std::cout << "Serialize(T&&)" << std::endl;
        auto desc = GetDescription(obj);
        SerializeType(desc.GetTypeName());
        for (const auto& item : desc)
        {
            SerializeField(item.first, item.second);
        }
    }
}
