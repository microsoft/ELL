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
    template<typename T>
    std::unique_ptr<T> TypeFactory::Construct(const std::string& typeName) const
    {
        auto entry = _typeMap.find(typeName);
        if (entry == _typeMap.end())
        {
            throw std::runtime_error("type " + typeName + " not registered in TypeFactory");
        }

        auto ptr = static_cast<T*>(entry->second());
        return std::unique_ptr<T>(ptr);
    }

    template<typename T>
    void TypeFactory::Add()
    {
        std::string typeName = T::GetTypeName();
        Add<T>(typeName);
    }

    template<typename T>
    void TypeFactory::Add(const std::string& typeName)
    {
        _typeMap[typeName] = []() -> void* { return new T; };
    }
}
