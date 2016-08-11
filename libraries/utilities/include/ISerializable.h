////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeName.h"
#include "Serializer.h"
#include "Exception.h"

// stl
#include <string>
#include <ostream>

namespace utilities
{
    /// <summary> ISerializable interface </summary>
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;

        /// <summary> Serializes the object. </summary>
        ///
        /// <param name="serializer">  The serializer. </param>
        virtual void Serialize(Serializer& serializer) const = 0;

        /// <summary> Deserializes the object. </summary>
        ///
        /// <param name="serializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(Deserializer& serializer, SerializationContext& context) { throw LogicException(LogicExceptionErrors::notImplemented); }
    };
}
