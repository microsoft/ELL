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
    template <typename ValueType, IsNotSerializable<ValueType> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
//        std::cout << "GetDescription(T&&)" << std::endl;
        ObjectDescription description = ObjectDescription::FromType(obj);
        return description;
    }

    template <typename ValueType, IsFundamental2<ValueType> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
//        std::cout << "GetDescription(fundamental)" << std::endl;
        ObjectDescription description = ObjectDescription::FromType(obj);
        return description;
    }

    inline ObjectDescription GetDescription(const ISerializable& obj)
    {
//        std::cout << "GetDescription(ISerializable)" << std::endl;
        auto result = obj.GetDescription();
        return result;
    }

    inline ObjectDescription GetDescription(const Variant& obj)
    {
//        std::cout << "GetDescription(Variant)" << std::endl;
        auto result = obj.GetObjectDescription();
        return result;
    }

    template <typename T>
    void Serializer::Serialize(T&& obj)
    {
//        std::cout << "Serialize(T&&) -- " << typeid(T).name() << std::endl;
        auto desc = GetDescription(obj);

        SerializeType(desc.GetTypeName());
        for (const auto& item : desc)
        {
            SerializeField(item.first, item.second);
        }
    }
}
