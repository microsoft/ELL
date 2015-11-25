//// SerializableIndexValue.h
//
//#pragma once
//
//#include "Mapping.h"
//
//#include "types.h"
//
//namespace mappings
//{
//    /// Represents an index value pair
//    ///
//    class SerializableIndexValue : public indexValue
//    {
//    public:
//
//        /// Default Ctor
//        ///
//        SerializableIndexValue() = default;
//
//        /// Copy Ctor
//        ///
//        SerializableIndexValue(const indexValue& other);
//
//        /// Serializes an index value pair
//        ///
//        void Serialize(JsonSerializer& serializer) const;
//
//        /// Deserializes an index value pair
//        ///
//        void Deserialize(JsonSerializer& serializer);
//    };
//}