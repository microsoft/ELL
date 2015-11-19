// Coordinatewise.h
#pragma once

#include "Mapping.h"
#include "IndexValue.h"

#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace mappings
{
    class Coordinatewise : public Mapping
    {
    public:

        /// Constructs a default Mapping
        ///
        Coordinatewise(function<double(double,double)> func);

        /// Constructs a Coordinatewise Mapping from index value pairs
        template <typename IndexValueIterator>
        Coordinatewise(IndexValueIterator begin, IndexValueIterator end, function<double(double, double)> func);

        using IndexValueIterator = vector<IndexValue>::const_iterator;

        /// TODO
        ///
        IndexValueIterator begin() const;

        ///
        ///
        IndexValueIterator end() const;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
        ///    
        virtual int GetOutputDim() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _current_version);

    protected:
        vector<IndexValue> _index_values;
        function<double(double, double)> _func;
        static const int _current_version = 1;
    };
}


