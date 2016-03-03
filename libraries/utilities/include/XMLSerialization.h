////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// stl
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    class XMLSerializer
    {
    public:
        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        template<typename ElementType>
        void Serialize(const char* name, const std::vector<ElementType>& value);

        template<typename ValueType>
        void Serialize(const char* name, const std::shared_ptr<ValueType>& spValue);

        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

        void WriteToStream(std::ostream& os) const;
        
    private:

        void Indent();

        uint64 _indentation = 0;
        std::stringstream _stream;
    };

    class XMLDeserializer
    {
    public:
        XMLDeserializer(std::istream& is);

        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        template<typename ElementType>
        void Deserialize(const char* name, std::vector<ElementType>& value);

        template<typename ValueType>
        void Deserialize(const char* name, std::shared_ptr<ValueType>& spValue);

        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

    private:
        std::string _string;
        const char* _pStr;
    };
}

#include "../tcc/XMLSerialization.tcc"
