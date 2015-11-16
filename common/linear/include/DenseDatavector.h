// DenseDataVector.h

#pragma once

#include "IDataVector.h"
#include "IndexValue.h"

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#define DEFAULT_DENSE_VECTOR_CAPACITY 1000

namespace linear
{
    /// DenseDataVector Base class
    ///
    template<typename ValueType>
    class DenseDataVector : public IDataVector
    {
    public:

        /// A read-only forward iterator for the sparse binary vector.
        ///
        class Iterator // TODO: implement this type
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

            // define typenames to improve readability
            using StlIteratorType = typename vector<ValueType>::const_iterator;

            /// private ctor, can only be called from SparseDataVector class
            Iterator(const StlIteratorType& begin, const StlIteratorType& end);
            friend DenseDataVector<ValueType>;

            // members
            StlIteratorType _begin;
            StlIteratorType _end;
            uint _index;
        };


        /// Constructor
        ///
        DenseDataVector();

        /// Converting constructor
        ///
        explicit DenseDataVector(const IDataVector& other);

        /// Move constructor
        ///
        DenseDataVector(DenseDataVector&& other) = default;

        /// Deleted copy constructor
        ///
        DenseDataVector(const DenseDataVector&) = delete;

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

        /// \Returns a Iterator that points to the beginning of the list.
        ///
        Iterator GetIterator() const;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(ostream& os) const override;

    private:
        uint _num_nonzeros;
        vector<ValueType> _mem;
    };

    class FloatDataVector : public DenseDataVector<float> 
    {
    public:
        using DenseDataVector<float>::DenseDataVector;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };

    class DoubleDataVector : public DenseDataVector<double>
    {
    public:
        using DenseDataVector<double>::DenseDataVector;

        /// \returns The type of the vector
        ///
        virtual type GetType() const override;
    };
}


