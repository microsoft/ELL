// Row.h

#pragma once

#include "Mapping.h"
#include <vector>
#include <memory>
#include <exception>

using std::vector;
using std::shared_ptr;
using std::exception;

namespace mappings
{
    /// An interface for a real-valued mathematical function from reals^m to reals^n
    ///
    class Row : public Mapping
    {
    public:

        /// Default ctor
        ///
        Row();

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void apply(const double* input, double* output) const;

        /// \returns The input dimension of the row. Namely, the apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const;

        /// \returns The output dimension of the row. Namely, the apply function assumes that the output array is at least this long
        ///
        virtual int GetOutputDim() const;

        void PushBack(shared_ptr<Mapping> m);

        const shared_ptr<Mapping> operator[] (int index) const;

        /// Serializes the row in json format
        ///
        virtual void Serialize(JsonSerializer& js) const;

        /// Deserializes the row in json format
        ///
        virtual void Deserialize(JsonSerializer& js, int version = _current_version);

    protected:
        vector<shared_ptr<Mapping>> _row_elements;
        int _in_dim;
        int _out_dim;
        static const int _current_version = 1;
    };
}
