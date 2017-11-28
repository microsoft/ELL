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
    std::string TypeName<T, std::enable_if_t<HasGetTypeName<std::decay_t<T>>::value>>::GetName()
    {
        return std::string(std::decay_t<T>::GetTypeName());
    };

    template <typename T>
    std::string TypeName<T, std::enable_if_t<std::is_enum<std::decay_t<T>>::value>>::GetName()
    {
        return "enum";
    };

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

    template <typename T>
    std::string TypeName<const std::vector<T>&>::GetName()
    {
        return GetCompositeTypeName<T>("vector");
    }

    template <typename Type>
    std::string GetTypeName()
    {
        return TypeName<typename std::decay_t<Type>>::GetName();
    }

    template <typename... Types>
    std::string GetCompositeTypeName(std::string baseType)
    {
        auto typeStrings = std::vector<std::string>{ TypeName<Types>::GetName()... };
        return GetCompositeTypeName(baseType, typeStrings);
    }
}
}
