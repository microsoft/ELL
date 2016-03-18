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

    class TypeFactory
    {
    public:

        template<typename T>
        std::unique_ptr<T> Construct(const std::string& typeName) const;

        template<typename T>
        void Add();

        template<typename T>
        void Add(const std::string& typeName);

    private:
        std::unordered_map<std::string, std::function<void*()>> _typeMap;
    };
}

#include "../tcc/TypeFactory.tcc"
