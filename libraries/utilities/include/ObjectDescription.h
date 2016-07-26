////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant_def.h"
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
        ObjectDescription(bool isFundamentalType): _typeName("[none]"), _isFundamentalType(isFundamentalType) {};

        ObjectDescription(const ObjectDescription& other) = default;

       template <typename ValueType>
       static ObjectDescription FromType();

       template <typename ValueType>
       static ObjectDescription FromType(ValueType&& obj);
        
        /// <summary> Adds an entry to the ObjectDescription </summary>
        template <typename ValueType>
        void AddField(std::string name, ValueType&& value);

        bool IsFundamentalType() const { return _isFundamentalType; }
        const Variant& GetFundamentalType() const;

        void Print(std::ostream& os, size_t indent = 0);
        std::string GetTypeName() const { return _typeName; }

        size_t GetNumFields() const { return _description.size(); }

        std::unordered_map<std::string, Variant>::iterator begin() { return _description.begin(); }
        std::unordered_map<std::string, Variant>::iterator end() { return _description.end(); }
        
    private:
        bool _isFundamentalType;
        std::string _typeName = "[none]";
        std::unordered_map<std::string, Variant> _description;
    };
}

#include "../tcc/ObjectDescription.tcc"
