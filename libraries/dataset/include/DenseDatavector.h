// DenseDataVector.h

#pragma once

#include "IDataVector.h"

#include "types.h"
#include "RealArray.h"
using common::RealArray;

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include <type_traits>
using std::is_base_of;
using std::enable_if_t;

#define DEFAULT_DENSE_VECTOR_CAPACITY 1000

namespace dataset
{
    /// DenseDataVector Base class
    ///
    template<typename ValueType>
    class DenseDataVector : public IDataVector
    {
    public:
        /// Constructor
        ///
        DenseDataVector();

        /// Converting constructor
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        DenseDataVector(IndexValueIteratorType indexValueIterator);

        /// Move constructor
        ///
        DenseDataVector(DenseDataVector&& other) = default;

        /// Deleted copy constructor
        ///
        DenseDataVector(const DenseDataVector&) = delete;

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
        
        /// Computes the Dot product
        ///
        virtual double Dot(const double* p_other) const override;

        /// \Returns An Iterator that points to the beginning of the vector.
        ///
        typename RealArray<ValueType>::Iterator GetIterator() const;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(ostream& os) const override;

    private:
        uint64 _num_nonzeros;
        RealArray<ValueType> _data;
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

#include "../tcc/DenseDatavector.tcc"


