// Row.h

#pragma once

#include "Mapping.h"

#include <vector>
using std::vector;

#include <memory>

using std::shared_ptr;

#include <exception>
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
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the row. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual uint64 GetMinInputDim() const;

        /// \returns The output dimension of the row. Namely, the Apply function assumes that the output array is at least this long
        ///
        virtual uint64 GetOutputDim() const;

        void PushBack(shared_ptr<Mapping> m);

        const shared_ptr<Mapping> operator[] (int index) const;

        /// Serializes the row in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the row in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion);

    protected:
        vector<shared_ptr<Mapping>> _rowElements;
        uint64 _inDim;
        uint64 _outDim;
        static const int _currentVersion = 1;
    };
}
