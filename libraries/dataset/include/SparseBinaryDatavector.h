// SparseBinaryDataVector.h

#pragma once

#include "IDataVector.h"
#include "IntegerList.h"
#include "CompressedIntegerList.h"

#include "types.h"

namespace dataset
{
    /// Implements a sparse binary vector as an increasing list of the coordinates where the value is 1.0
    ///
    template<typename IntegerListType>
    class SparseBinaryDataVectorBase : public IDataVector
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

            /// \returns The current index
            ///
            uint64 GetIndex() const; // TODO: replace with IndexValue class as return type

            /// \returns The current value
            ///
            double Get() const;

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
        template<typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        SparseBinaryDataVectorBase(IndexValueIteratorType IndexValueIterator);

        /// Move constructor
        ///
        SparseBinaryDataVectorBase(SparseBinaryDataVectorBase<IntegerListType>&& other) = default;

        /// Deleted copy constructor
        ///
        SparseBinaryDataVectorBase(const SparseBinaryDataVectorBase<IntegerListType>& other) = delete;

        /// Sets the element at the given index to 1.0. Calls to this function must have a monotonically increasing argument. 
        /// The value argument must equal 1.0
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

#include "../tcc/SparseBinaryDataVector.tcc"


