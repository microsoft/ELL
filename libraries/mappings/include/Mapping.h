//// Mapping.h
//
//#pragma once
//
//#include "types.h"
//
//#include "JsonSerializer.h"
//using utilities::JsonSerializer;
//
//#include <string>
//using std::string;
//
//#include <vector>
//using std::vector;
//
//namespace mappings
//{
//    /// An interface for a real-valued mathematical function from reals^m to reals^n
//    ///
//    class Mapping
//    {
//    public:
//        enum class Types { constant, scale, shift, sum, decisionTreePath, row, column};
//
//        /// \returns The type of Mapping
//        ///
//        Types GetType() const;
//    
//        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
//        ///
//        virtual void Apply(const double* input, double* output) const = 0;
//
//        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
//        ///
//        void Apply(const vector<double>& input, vector<double>& output) const;
//
//        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
//        ///
//        virtual uint64 GetMinInputDim() const = 0;
//
//        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
//        ///
//        virtual uint64 GetOutputDim() const = 0;
//
//        /// Serializes the Mapping in json format
//        ///
//        virtual void Serialize(JsonSerializer& serializer) const = 0;
//
//        /// Deserializes the Mapping in json format
//        ///
//        virtual void Deserialize(JsonSerializer& serializer, int version) = 0;
//        
//    protected:
//        Types _type;
//
//        void SerializeHeader(JsonSerializer& serializer, int version) const;
//    };
//}
