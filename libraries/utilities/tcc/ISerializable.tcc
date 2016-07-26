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
        // throw exception?
        return description;
    }

    template <typename ValueType, IsFundamental2<ValueType> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
//        std::cout << "GetDescription(fundamental)" << std::endl;
        ObjectDescription description = ObjectDescription::FromType(obj);
        description.AddField("value", obj);
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

    //
    // Serialize
    //

    // TODO: just overload Serialize() on fundamental types instead of creating descriptions for them, etc.
    template <typename T>
    void Serializer::Serialize(T&& obj)
    {
        auto desc = GetDescription(obj);
        if(desc.IsFundamentalType())
        {
            SerializeFundamentalType(desc.GetFundamentalType());
        }
        else
        {
            BeginSerializeType(desc);
            int index = desc.GetNumFields();
            
            for (const auto& item : desc)
            {
                --index;
                SerializeField(item.first, item.second);
                if(index > 0) std::cout << ",";
                std::cout << std::endl;
            }
            EndSerializeType(desc);
        }
    }
}
