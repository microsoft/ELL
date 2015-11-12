// DoubleSubvectorCref.h

#pragma once

#include "IVector.h"
#include "DoubleVector.h"

#include <functional>
using std::function;

#include <vector>
using std::vector;

#include <cassert> // for assert
#include <cstdint> // for UINT64_MAX

namespace linear
{
    /// DoubleSubvectorCref class
    ///
    class DoubleSubvectorCref : public IVector
    {
    public:

        /// Constructs a Constant reference to a double vector
        ///
        DoubleSubvectorCref(const DoubleVector& vec, uint offset = 0, uint size = UINT64_MAX);

        /// Constructs a Constant reference to a double vector
        ///
        DoubleSubvectorCref(const double* ptr, uint size);

        /// Computes the vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other is a dense vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// Prints the vector to an output stream
        ///
        virtual void Print(ostream & os) const override;

        /// \returns The Size of the vector
        ///
        virtual uint Size() const override;

    private:
        const double* _ptr;
        uint _size;
    };
}


