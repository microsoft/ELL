// DoubleVector.h

#pragma once

#include "IVector.h"

#include <functional>
using std::function;

#include <vector>
using std::vector;

#include <cassert>

namespace linear
{
    /// DoubleVector class
    ///
    class DoubleVector : public vector<double>, public IVector
    {
    public:

        /// A read-only forward iterator for the sparse binary vector.
        ///
        class Iterator : public IIndexValueIterator
        {
        public:

            /// Default copy ctor
            ///
            Iterator(const Iterator&) = default;

            /// Default move ctor
            ///
            Iterator(Iterator&&) = default;

            /// \returns True if the iterator is currently pointing to a valid iterate
            ///
            bool IsValid() const;

            /// Proceeds to the Next iterate
            ///
            void Next();

            /// \returns The current index-value pair
            ///
            indexValue Get() const;

        private:

            /// private ctor, can only be called from SparseDataVector class
            Iterator(const vector<double>::const_iterator& begin, const vector<double>::const_iterator& end);
            friend DoubleVector;

            // members
            vector<double>::const_iterator _begin;
            vector<double>::const_iterator _end;
            uint64 _index = 0;
        };

        using vector<double>::vector;
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

        /// \Returns a Iterator that points to the beginning of the list.
        ///
        Iterator GetIterator() const;

        /// Prints the vector to an output stream
        ///
        virtual void Print(ostream & os) const override;
    };
}


