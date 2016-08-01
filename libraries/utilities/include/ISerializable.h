////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeName.h"
#include "Serialization.h"
#include "Exception.h"

// stl
#include <string>
#include <ostream>
#include <type_traits>

namespace utilities
{
    /// <summary> ISerializable interface --- provides one function, GetDescription </summary>
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        virtual std::string GetRuntimeTypeName() const = 0;

        virtual void Serialize(Serializer& serializer) const = 0;  // TODO: call this SerializeContents?
        virtual void BeginSerialize(Serializer& serializer) const {};
        virtual void EndSerialize(Serializer& serializer) const {};

        ///
        virtual void Deserialize(Deserializer& serializer, SerializationContext& context) { throw LogicException(LogicExceptionErrors::notImplemented); }
        virtual void BeginDeserialize(Deserializer& serializer, SerializationContext& context) {};
        virtual void EndDeserialize(Deserializer& serializer, SerializationContext& context) {};
    };
}

#include "../tcc/ISerializable.tcc"
