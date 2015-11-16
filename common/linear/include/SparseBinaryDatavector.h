// SparseBinaryDataVector.h

#pragma once

#include "types.h"
#include "IDataVector.h"
#include "IntegerList.h"
#include "CompressedIntegerList.h"

namespace linear
{
    /// Implements a sparse binary vector as an increasing list of the coordinates where the value is 1.0
    ///
    template<typename IntegerListType>
    class SparseBinaryDataVectorBase : public IDataVector
    {
    public:

        /// A read-only forward iterator for the sparse binary vector.
        ///
        class Iterator
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

            /// \returns The current index
            ///
            uint GetIndex() const; // TODO: replace with IndexValue class as return type

            /// \returns The current value
            ///
            double GetValue() const;

        private:

            // define typename to improve readability
            using IndexIteratorType = typename IntegerListType::Iterator;

            // private ctor, can only be called from SparseBinaryDataVectorBase class
            Iterator(const typename IntegerListType::Iterator& list_iterator);
            friend SparseBinaryDataVectorBase<IntegerListType>;

            // members
            IndexIteratorType _list_iterator;
        };

        /// Constructs an empty sparse binary vector
        ///
        SparseBinaryDataVectorBase();

        /// Converting constructor
        ///
        explicit SparseBinaryDataVectorBase(const IDataVector& other);

        /// Move constructor
        ///
        SparseBinaryDataVectorBase(SparseBinaryDataVectorBase<IntegerListType>&& other) = default;

        /// Deleted copy constructor
        ///
        SparseBinaryDataVectorBase(const SparseBinaryDataVectorBase<IntegerListType>& other) = delete;

        /// Sets the element at the given index to 1.0. Calls to this function must have a monotonically increasing argument. 
        /// The value argument must equal 1.0
        virtual void PushBack(uint index, double value = 1.0) override;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() override;

        /// Calls a callback function for each non-zero entry in the vector, in order of increasing index
        ///
        //virtual    void foreach_nonzero(function<void(uint, double)> func, uint index_offset = 0) const override; TODO

        /// \returns The largest index of a non-zero entry plus one
        ///
        virtual uint Size() const override;

        /// \returns The number of non-zeros
        ///
        virtual uint NumNonzeros() const override;

        /// Computes the vector squared 2-norm
        ///
        virtual double Norm2() const override;

        /// Performs (*p_other) += scalar * (*this), where other a dense vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// \Returns a Iterator that traverses the non-zero entries of the sparse vector
        ///
        Iterator GetIterator() const;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(ostream& os) const override;

    private:
        IntegerListType _indices;
    };

    class  SparseBinaryDataVector : public SparseBinaryDataVectorBase<CompressedIntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<CompressedIntegerList>::SparseBinaryDataVectorBase;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };

    class UncompressedSparseBinaryVector : public SparseBinaryDataVectorBase<IntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<IntegerList>::SparseBinaryDataVectorBase;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };
}

