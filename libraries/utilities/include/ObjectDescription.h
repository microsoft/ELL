////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"
#include "TypeName.h"

// stl
#include <string>
#include <ostream>
#include <unordered_map>

namespace utilities
{
    class ObjectDescription
    {
    public:
        ObjectDescription(): _typeName("[none]") {};

        template <typename ValueType>
        ObjectDescription(ValueType&& obj);

        /// <summary> Adds an entry to the ObjectDescription </summary>
        template <typename ValueType>
        void AddField(std::string name, ValueType&& value);

        void Print(std::ostream& os, size_t indent = 0);
        std::string GetTypeName() { return _typeName; }

        std::unordered_map<std::string, Variant>::iterator begin() { return _description.begin(); }
        std::unordered_map<std::string, Variant>::iterator end() { return _description.end(); }
        
    private:
        std::string _typeName = "[none]";
        std::unordered_map<std::string, Variant> _description;
    };
}

#include "../tcc/ObjectDescription.tcc"
