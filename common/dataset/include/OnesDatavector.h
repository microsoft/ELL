// OnesDataVector.h

#pragma once

#include "IDataVector.h"

#include "types.h"
using linear::uint;

#include "IndexValue.h"
using linear::IndexValue;
using linear::IIndexValueIterator;

namespace dataset
{
    /// OnesDataVector Base class
    ///
    class OnesDataVector : public IDataVector
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
            IndexValue GetValue() const;

        private:
            /// private ctor, can only be called from SparseDataVector class
            Iterator(uint size);
            friend OnesDataVector;

            // members
            uint _size = 0;
            uint _index = 0;
        };

        /// Constructor
        ///
        OnesDataVector(uint dim = 0);

        /// Converting constructor
        ///
        explicit OnesDataVector(const IDataVector& other);

        /// Move constructor
        ///
        OnesDataVector(OnesDataVector&& other) = default;

        /// Deleted copy constructor
        ///
        OnesDataVector(const OnesDataVector&) = delete;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;

        /// Sets an entry in the vector
        ///
        virtual void PushBack(uint index, double value = 1.0) override;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() override;

        /// Calls a callback function for each non-zero entry in the vector, in order of increasing index
        ///
        //virtual    void foreach_nonzero(function<void(uint, double)> func, uint index_offset = 0) const override;

        /// \returns The largest index of a non-zero entry plus one
        ///
        virtual uint Size() const override;

        /// \returns The number of non-zeros
        ///
        virtual uint NumNonzeros() const override;

        /// Computes the vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other is a dense vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;
        
        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// \Returns a Iterator that points to the beginning of the datavector.
        ///
        Iterator GetIterator() const;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(ostream& os) const override;

    private:
        uint _size = 0;
    };
}