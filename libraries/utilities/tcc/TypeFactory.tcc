////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TypeFactory.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename BaseType>
    std::unique_ptr<BaseType> TypeFactory<BaseType>::Construct(const std::string& typeName) const
    {
        auto entry = _typeMap.find(typeName);
        if (entry == _typeMap.end())
        {
            throw std::runtime_error("type " + typeName + " not registered in TypeFactory<" + BaseType::GetTypeName() + ">");
        }

        return entry->second();
    }

    template<typename BaseType>
    template<typename RuntimeType>
    void TypeFactory<BaseType>::AddType()
    {
        std::string typeName = RuntimeType::GetTypeName();
        AddType<RuntimeType>(typeName);
    }

    template<typename BaseType>
    template<typename RuntimeType>
    void TypeFactory<BaseType>::AddType(const std::string& typeName)
    {
        static_assert(std::is_base_of<BaseType, RuntimeType>::value, "incompatible base and runtime types in TypeFactory::Add");

        _typeMap[typeName] = []() { return static_cast<BaseType>(std::make_unique<RuntimeType>()); };
    }
}
