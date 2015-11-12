// Column.h

#pragma once

#include "Mapping.h"
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

namespace mappings
{
    /// An interface for a real-valued mathematical function from reals^m to reals^n
    ///
    class Column : public Mapping
    {
    public:

        /// Default ctor
        ///
        Column();

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector)
        ///
        virtual void apply(const double* input, double* output) const;
        
        /// \returns The input dimension of the column. Namely, the apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const;

        /// \returns The output dimension of the column. Namely, the apply function assumes that the output array is at least this long
        ///
        virtual int GetOutputDim() const;

        /// Adds a Mapping to the bottom of the column
        ///
        void PushBack(shared_ptr<Mapping> m);

        /// \returns A reference to one of the mappings in the column
        ///
        shared_ptr<Mapping> operator[] (int index);

        /// \returns A reference to one of the mappings in the column
        ///
        const shared_ptr<Mapping> operator[] (int index) const;

        /// Serializes the column in json format
        ///
        virtual void Serialize(JsonSerializer& js) const;

        /// Deserializes the column in json format
        ///
        virtual void Deserialize(JsonSerializer& js, int version = _current_version);

        /// KeepLayers(0) keeps the entire Mapping
        /// KeepLayers(positive) keeps only the specified number of layers, starting from the top
        /// KeepLayers(negative) subtracts the specified number of layers, starting from the bottom
        void KeepLayers(int num = 0);

    protected:
        vector<shared_ptr<Mapping>> _column_elements;
        int _max_internal_dim;
        static const int _current_version = 1;

    };
}
