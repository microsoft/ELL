// DoubleVector.h

#pragma once

#include "IVector.h"

#include "types.h"
#include "RealArray.h"
using types::DoubleArray;

namespace linear
{
    /// DoubleVector class
    ///
    class DoubleVector : public DoubleArray, public IVector
    {
    public:

        /// Ctor
        ///
        DoubleVector(uint64 size = 0);

        /// Default copy ctor
        ///
        DoubleVector(const DoubleVector&) = default;

        /// Default move ctor
        ///
        DoubleVector(DoubleVector&&) = default;

        /// Converting constructor
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        DoubleVector(IndexValueIteratorType indexValueIterator) : DoubleArray(indexValueIterator) {}

        using IVector::AddTo;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        void Reset();

        /// Computes the vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other is a dense vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// Scales the vector by a scalar
        ///
        void Scale(double s);

        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// \returns The Size of the vector
        ///
        virtual uint64 Size() const override;

        /// Prints the vector to an output stream
        ///
        virtual void Print(ostream& os) const override;
    };
}



