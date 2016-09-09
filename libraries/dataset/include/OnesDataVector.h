////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OnesDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// utilities
#include "IndexValue.h"

// stl
#include <cstdint>
#include <iostream>

namespace emll
{
namespace dataset
{
    /// <summary> OnesDataVector Base class. </summary>
    class OnesDataVector : public IDataVector
    {
    public:
        /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
        class Iterator : public linear::IIndexValueIterator
        {
        public:
            Iterator(const Iterator&) = default;

            Iterator(Iterator&&) = default;

            /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return _index < _size; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next() { ++_index; }

            /// <summary> Gets the current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const { return linear::IndexValue{ _index, 1 }; }

        private:
            // private ctor, can only be called from SparseDataVector class
            Iterator(uint64_t size);
            friend OnesDataVector;

            // members
            uint64_t _size = 0;
            uint64_t _index = 0;
        };

        /// <summary> Constructs an instance of OnesDataVector. </summary>
        ///
        /// <param name="dim"> The dimension of the DataVector. </param>
        OnesDataVector(uint64_t dim = 0);

        OnesDataVector(OnesDataVector&& other) = default;

        OnesDataVector(const OnesDataVector&) = default;

        /// <summary> Sets an entry in the vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) override;

        /// <summary> Deletes all of the vector content and sets its Size to zero, but does not deallocate
        /// its memory. </summary>
        virtual void Reset() override { _size = 0; }

        /// <summary> Returns the size of the DataVector, which is the largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> The size of the DataVector. </returns>
        virtual uint64_t Size() const override { return _size; }

        /// <summary> The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const override { return _size; }

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override { return (double)_size; }

        /// <summary> Performs (*p_other) += scalar * (*this), where other is a dense vector. </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;
        using IVector::AddTo;

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const override;
        using IVector::Dot;

        /// <summary> Returns an Iterator that points to the beginning of the datavector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const { return Iterator(_size); }

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

        /// <summary> Makes a deep copy of the datavector </summary>
        ///
        /// <returns> A deep copy of the datavector </summary>
        virtual std::unique_ptr<IDataVector> Clone() const override { return std::make_unique<OnesDataVector>(*this); }

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override;

    private:
        uint64_t _size = 0;
    };
}
}
