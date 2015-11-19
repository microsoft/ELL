// Constant.h
#pragma once

#include "Mapping.h"

namespace mappings
{
    class Constant : public Mapping
    {
    public:

        /// Constructs a default Mapping
        ///
        Constant(double c = 0.0);

        /// Sets the output constant
        ///
        void SetConstant(double c);
        
        /// \returns The output constant
        ///
        double GetConstant() const;
        
        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual uint64 GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
        ///    
        virtual uint64 GetOutputDim() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion);

    protected:
        double _c;
        static const int _currentVersion = 1;
    };
}
