// ZeroDataVector.h

#pragma once

#include "IDataVector.h"

// types
#include "types.h"

namespace dataset
{
    /// ZeroDataVector Base class
    ///
    class ZeroDataVector : public IDataVector
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
            IndexValue Get() const;

        private:
            /// private ctor, can only be called from SparseDataVector class
            Iterator() = default;
            friend ZeroDataVector;
        };

        /// Constructor
        ///
        ZeroDataVector() = default;

        /// Converting constructor
        ///
        explicit ZeroDataVector(const IDataVector& other);

        /// Move constructor
        ///
        ZeroDataVector(ZeroDataVector&& other) = default;

        /// Deleted copy constructor
        ///
        ZeroDataVector(const ZeroDataVector&) = delete;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;

        /// Sets an entry in the vector
        ///
        virtual void PushBack(uint64 index, double value = 1.0) override;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() override;

        /// \returns The largest index of a non-zero entry plus one
        ///
        virtual uint64 Size() const override;

        /// \returns The number of non-zeros
        ///
        virtual uint64 NumNonzeros() const override;

        /// Computes the vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other is a dense vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;
        using IVector::AddTo;

        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;
        using IVector::Dot;

        /// \Returns a Iterator that points to the beginning of the datavector.
        ///
        Iterator GetIterator() const;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(ostream& os) const override;
    };
}
