////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeName.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename T>
    std::string TypeName<T*>::GetName()
    {
        return GetCompositeTypeName<T>("ptr");
    }

    template <typename T>
    std::string TypeName<std::unique_ptr<T>>::GetName()
    {
        return GetCompositeTypeName<T>("unique_ptr");
    }

    template <typename T>
    std::string TypeName<std::vector<T>>::GetName()
    {
        return GetCompositeTypeName<T>("vector");
    }

    template <typename... Types>
    std::string GetCompositeTypeName(std::string baseType)
    {
        auto typeStrings = std::vector<std::string>{ TypeName<Types>::GetName()... };
        return GetCompositeTypeName(baseType, typeStrings);
    }
}
}
