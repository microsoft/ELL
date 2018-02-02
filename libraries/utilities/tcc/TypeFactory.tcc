////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeFactory.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Debug.h"

namespace ell
{
namespace utilities
{
    template <typename BaseType>
    std::unique_ptr<BaseType> TypeFactory<BaseType>::Construct(const std::string& typeName) const
    {
        auto entry = _typeMap.find(typeName);
        if (entry == _typeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "type " + typeName + " not registered in TypeFactory<" + BaseType::GetTypeName() + ">");
        }

        return entry->second();
    }

    template <typename BaseType>
    template <typename RuntimeType>
    void TypeFactory<BaseType>::AddType()
    {
        std::string typeName = RuntimeType::GetTypeName();
        AddType<RuntimeType>(typeName);
    }

    template <typename BaseType>
    template <typename RuntimeType>
    void TypeFactory<BaseType>::AddType(const std::string& typeName)
    {
        static_assert(std::is_base_of<BaseType, RuntimeType>::value, "incompatible base and runtime types in TypeFactory::Add");

        DEBUG_THROW(_typeMap.find(typeName) != _typeMap.end(), std::logic_error(typeName + " has already been added to the type factory"));

        _typeMap[typeName] = []() -> std::unique_ptr<BaseType> { return (std::make_unique<RuntimeType>()); };
    }

    //
    // GenericTypeFactory
    //
    template <typename BaseType>
    class TypeConstructorDerived : public TypeConstructorBase
    {
    public:
        template <typename RuntimeType>
        static std::unique_ptr<TypeConstructorDerived<BaseType>> NewTypeConstructor()
        {
            auto result = std::make_unique<TypeConstructorDerived<BaseType>>();
            result->_createFunction = []() {
                auto runtimePtr = new RuntimeType();
                auto basePtr = dynamic_cast<BaseType*>(runtimePtr);
                return std::unique_ptr<BaseType>(basePtr);
            };
            return result;
        }

        std::unique_ptr<BaseType> Construct() const
        {
            return _createFunction();
        }

    private:
        std::function<std::unique_ptr<BaseType>()> _createFunction;
    };

    //
    // TypeConstructorBase implementation
    //
    template <typename BaseType>
    std::unique_ptr<BaseType> TypeConstructorBase::Construct() const
    {
        auto thisPtr = dynamic_cast<const TypeConstructorDerived<BaseType>*>(this);
        if (thisPtr == nullptr)
        {
            throw InputException(InputExceptionErrors::typeMismatch, std::string{ "TypeConstructorBase::Construct called with wrong type. BaseType: " } + BaseType::GetTypeName());
        }

        return thisPtr->Construct();
    }

    //
    // GenericTypeFactory implementation
    //
    template <typename BaseType>
    std::unique_ptr<BaseType> GenericTypeFactory::Construct(const std::string& typeName) const
    {
        auto baseTypeName = std::string{ BaseType::GetTypeName() };
        auto key = baseTypeName + "__" + typeName;
        auto entry = _typeConstructorMap.find(key);
        if (entry == _typeConstructorMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "type " + typeName + " not registered in TypeFactory<" + BaseType::GetTypeName() + ">");
        }

        return entry->second->Construct<BaseType>();
    }

    template <typename BaseType, typename RuntimeType>
    void GenericTypeFactory::AddType()
    {
        auto typeName = RuntimeType::GetTypeName();
        AddType<BaseType, RuntimeType>(typeName);
    }

    template <typename BaseType, typename RuntimeType>
    void GenericTypeFactory::AddType(const std::string& typeName)
    {
        auto baseTypeName = std::string{ BaseType::GetTypeName() };
        auto key = baseTypeName + "__" + typeName;

        DEBUG_THROW(_typeConstructorMap.find(key) != _typeConstructorMap.end(), std::logic_error(key + " has already been added to the type factory"));

        auto derivedCreator = TypeConstructorDerived<BaseType>::template NewTypeConstructor<RuntimeType>().release();
        _typeConstructorMap[key] = std::shared_ptr<TypeConstructorBase>(derivedCreator);
    }
}
}
