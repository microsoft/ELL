////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"

#include <unordered_map>
#include <string>
#include <ostream>

namespace utilities
{
    class ObjectDescription
    {
    public:
        /// <summary> Adds an entry to the ObjectDescription </summary>
        void AddField(std::string name, const Variant& value);
        
        void Print(std::ostream& os, size_t indent = 0);

    private:
        std::unordered_map<std::string, Variant> _description;
    };

    class ISerializable
    {
    public:
        /// <summary> Get an ObjectDescription describing how to serialize this object </summary>
        virtual ObjectDescription GetDescription() const = 0;
    };

    class Serializer
    {
    public:
        template <typename T>
        void Serialize(T&& obj);

    protected:
        virtual void SerializeField(std::string name, const Variant& variant) = 0;
    };

    class SimpleSerializer : public Serializer
    {
    protected:
        virtual void SerializeField(std::string name, const Variant& variant) override
        {

        }
    };
}
