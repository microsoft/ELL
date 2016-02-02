// DoubleSubvectorCref.h

#pragma once

#include "IVector.h"
#include "DoubleVector.h"

#include <functional>

#include <vector>

namespace linear
{
    /// DoubleSubvectorCref class
    ///
    class DoubleSubvectorCref : public IVector
    {
    public:

        /// Constructs a constant reference to a double std::vector
        ///
        DoubleSubvectorCref(const DoubleVector& vec, uint64 offset = 0, uint64 size = UINT64_MAX);

        /// Constructs a constant reference to a double std::vector
        ///
        DoubleSubvectorCref(const double* ptr, uint64 size);

        /// Computes the std::vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other is a dense std::vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// Prints the std::vector to an output stream
        ///
        virtual void Print(ostream & os) const override;

        /// \returns The Size of the std::vector
        ///
        virtual uint64 Size() const override;

    private:
        const double* _ptr;
        uint64 _size;
    };
}


