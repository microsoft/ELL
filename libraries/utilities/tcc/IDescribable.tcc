////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IDescribable.tcc (utilities)
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

    template <typename ValueType, IsFundamental<ValueType> concept>
    ObjectDescription GetDescription(ValueType&& obj)
    {
//        std::cout << "GetDescription(fundamental)" << std::endl;
        ObjectDescription description = ObjectDescription::FromType(obj);
        description.AddField("value", obj);
        return description;
    }

    inline ObjectDescription GetDescription(const IDescribable& obj)
    {
//        std::cout << "GetDescription(IDescribable)" << std::endl;
        auto result = obj.GetDescription();
        return result;
    }

    inline ObjectDescription GetDescription(const Variant& obj)
    {
//        std::cout << "GetDescription(Variant)" << std::endl;
        auto result = obj.GetObjectDescription();
        return result;
    }
}
