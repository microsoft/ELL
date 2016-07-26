////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable.h"
#include "Variant.h"

namespace utilities
{
    //
    // Private code we'd like to hide:
    //
    namespace VariantNamespace
    {
        template <typename ValueType>
        auto GetValueString(const ValueType& value, double) -> std::string
        {
            return std::string("[No to_string for type ") + typeid(ValueType).name() + "]";
        }

        using std::to_string;
        using utilities::to_string;
        inline std::string to_string(const std::string& str) { return str; }
        
        template <typename ValueType>
        auto GetValueString(const ValueType& value, int) -> decltype(to_string(value), std::string())
        {
            return to_string(value);
        }
    }

    template <typename ValueType>
    inline std::string VariantDerived<ValueType>::ToString() const
    {
        return VariantNamespace::GetValueString(_value, (int)0);
    }

    template <typename ValueType>
    inline std::string VariantDerived<ValueType>::GetStoredTypeName() const
    {
        // TODO: call GetRuntimeTypeName if we can
        return TypeName<ValueType>::GetName();
    }

    template <typename ValueType>
    ObjectDescription VariantDerived<ValueType>::GetObjectDescription() const
    {
        // std::cout << "type " << TypeName<ValueType>::GetName() << std::endl;
        // std::cout << "####" << std::endl;
        auto description = GetDescription(_value);
        return description;
    }
}
