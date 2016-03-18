////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TypeFactory.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <unordered_map>
#include <functional>
#include <string>
#include <memory>

namespace utilities
{
    template<typename BaseType>
    class TypeFactory
    {
    public:

        std::unique_ptr<BaseType> Construct(const std::string& typeName) const;

        template<typename RuntimeType>
        void AddType();

        template<typename RuntimeType>
        void AddType(const std::string& typeName);

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<BaseType>()>> _typeMap;
    };
}

#include "../tcc/TypeFactory.tcc"
