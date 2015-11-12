// Mapping.h

#pragma once

#include "JsonSerializer.h"
#include <string>
#include <vector>

using utilities::JsonSerializer;
using std::string;
using std::vector;

namespace mappings
{
    /// An interface for a real-valued mathematical function from reals^m to reals^n
    ///
    class Mapping
    {
    public:
        enum types { Constant, Scale, Shift, Sum, DecisionTreePath, Row, Column};

        /// \returns The type of Mapping
        ///
        types GetType() const;
    
        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void apply(const double* input, double* output) const = 0;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        void apply(const vector<double>& input, vector<double>& output) const;

        /// \returns The input dimension of the Mapping. Namely, the apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const = 0;

        /// \returns The output dimension of the Mapping. Namely, the apply function assumes that the output array is at least this long
        ///
        virtual int GetOutputDim() const = 0;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& js) const = 0;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& js, int version) = 0;
        
    protected:
        types _type;

        void SerializeHeader(JsonSerializer& js, int version) const;
    };
}
