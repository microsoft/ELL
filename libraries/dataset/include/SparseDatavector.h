// SparseDataVector.h

#pragma once

#include "IDataVector.h"
#include "CompressedIntegerList.h"

#include "types.h"






namespace dataset
{
    /// Implements a sparse binary vector as an increasing list of the coordinates where the value is 1.0
    ///
    template<typename ValueType, typename IntegerListType>
    class SparseDataVector : public IDataVector
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
            indexValue GetValue() const;

        private:
            
            // define typenames to improve readability
            using IndexIteratorType = typename IntegerListType::Iterator;
            using ValueIteratorType = typename vector<ValueType>::const_iterator;

            /// private ctor, can only be called from SparseDataVector class
            Iterator(const IndexIteratorType& list_iterator, const ValueIteratorType& value_iterator);
            friend SparseDataVector<ValueType, IntegerListType>;

            // members
            IndexIteratorType _index_iterator;
            ValueIteratorType _value_iterator;
        };

        /// Constructs an empty sparse binary vector
        ///
        SparseDataVector();

        /// Converting constructor
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        SparseDataVector(IndexValueIteratorType&& indexValueIterator);

        /// Move constructor
        ///
        SparseDataVector(SparseDataVector<ValueType, IntegerListType>&& other) = default;

        /// Deleted copy constructor
        ///
        SparseDataVector(const SparseDataVector<ValueType, IntegerListType>& other) = delete;

        /// Sets the element at the given index to 1.0. Calls to this function must have a monotonically increasing argument. 
        /// The value argument must equal 1.0
        virtual void PushBack(uint64 index, double value) override;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() override;

        /// Calls a callback function for each non-zero entry in the vector, in order of increasing index
        ///
        //virtual    void foreach_nonzero(function<void(uint64, double)> func, uint64 index_offset = 0) const override;

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
        vector<ValueType> _values;
    };

    class SparseDoubleDataVector : public SparseDataVector<double, CompressedIntegerList>
    {
    public:
        using SparseDataVector<double, CompressedIntegerList>::SparseDataVector;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };

    class SparseFloatDataVector : public SparseDataVector<float, CompressedIntegerList>
    {
    public:
        using SparseDataVector<float, CompressedIntegerList>::SparseDataVector;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };

    class SparseShortDataVector : public SparseDataVector<short, CompressedIntegerList>
    {
    public:
        using SparseDataVector<short, CompressedIntegerList>::SparseDataVector;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };
}

#include "../tcc/SparseDataVector.tcc"

