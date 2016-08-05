////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeFactory.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace utilities
{
    template<typename BaseType>
    std::unique_ptr<BaseType> TypeFactory<BaseType>::Construct(const std::string& typeName) const
    {
        auto entry = _typeMap.find(typeName);
        if (entry == _typeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "type " + typeName + " not registered in TypeFactory<" + BaseType::GetTypeName() + ">");
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

        _typeMap[typeName] = []() -> std::unique_ptr<BaseType> { return(std::make_unique<RuntimeType>()); };
    }


    //
    // GenericTypeFactory
    //
    class TypeCreatorBase
    {
    public:
        virtual ~TypeCreatorBase() {}

        template <typename BaseType>
        std::unique_ptr<BaseType> Create() const;
    };

    template <typename BaseType>
    class TypeCreatorDerived : public TypeCreatorBase
    {
    public:
        template <typename RuntimeType>
        static std::unique_ptr<TypeCreatorDerived<BaseType>> NewTypeCreator()
        {
            auto result = std::make_unique<TypeCreatorDerived<BaseType>>();            
            result->_createFn = []()
            {
                auto runtimePtr = new RuntimeType();
                auto basePtr = dynamic_cast<BaseType*>(runtimePtr);
                return std::unique_ptr<BaseType>(basePtr); 
            };
            return result;
        }

        std::unique_ptr<BaseType> Create() const
        {
            return _createFn();
        }

    private:
        std::function<std::unique_ptr<BaseType>()> _createFn;
    };

    //
    // TypeCreatorBase implementation
    //
    template <typename BaseType>
    std::unique_ptr<BaseType> TypeCreatorBase::Create() const
    {
        auto thisPtr = dynamic_cast<const TypeCreatorDerived<BaseType>*>(this);
        if (thisPtr == nullptr)
        {
            throw InputException(InputExceptionErrors::typeMismatch, std::string{"TypeCreatorBase::Create called with wrong type. BaseType: "} + BaseType::GetTypeName());
        }

        return thisPtr->Create();
    }

    //
    // GenericTypeFactory implementation
    //
    template <typename BaseType>
    std::unique_ptr<BaseType> GenericTypeFactory::Construct(const std::string& typeName) const
    {
        auto baseTypeName = std::string{BaseType::GetTypeName()};
        auto key = baseTypeName + "__" + typeName;
        auto entry = _typeCreatorMap.find(key);
        if (entry == _typeCreatorMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "type " + typeName + " not registered in TypeFactory<" + BaseType::GetTypeName() + ">");
        }

        return entry->second->Create<BaseType>();        
    }

    template<typename BaseType, typename RuntimeType>
    void GenericTypeFactory::AddType()
    {
        auto typeName = RuntimeType::GetTypeName();
        AddType<BaseType, RuntimeType>(typeName);
    }

    template<typename BaseType, typename RuntimeType>
    void GenericTypeFactory::AddType(const std::string& typeName)
    {
        auto baseTypeName = std::string{BaseType::GetTypeName()};
        auto key = baseTypeName + "__" + typeName;

        auto derivedCreator = TypeCreatorDerived<BaseType>::template NewTypeCreator<RuntimeType>().release();
        _typeCreatorMap[key] = std::unique_ptr<TypeCreatorBase>(derivedCreator);
    }
}
