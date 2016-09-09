////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ZeroDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// linear
#include "IndexValue.h"

// stl
#include <cstdint>

namespace emll
{
namespace dataset
{
    /// <summary> ZeroDataVector Base class. </summary>
    class ZeroDataVector : public IDataVector
    {
    public:
        /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
        class Iterator : public linear::IIndexValueIterator
        {
        public:
            Iterator(const Iterator&) = default;

            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return false; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next() {}

            /// <summary> Returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const;

        private:
            // private ctor, can only be called from SparseDataVector class
            Iterator() = default;
            friend ZeroDataVector;
        };

        ZeroDataVector() = default;

        ZeroDataVector(ZeroDataVector&& other) = default;

        ZeroDataVector(const ZeroDataVector&) = default;

        /// <summary> Sets an entry in the vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) override;

        /// <summary> Deletes all of the vector content and sets its Size to zero, but does not deallocate
        /// its memory. </summary>
        virtual void Reset() override {}

        /// <summary> Returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An uint64_t. </returns>
        virtual uint64_t Size() const override { return 0; }

        /// <summary> Returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const override { return 0; }

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override { return 0.0; }

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

        /// <summary> Returns a Iterator that points to the beginning of the datavector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const { return Iterator(); }

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

        /// <summary> Makes a deep copy of the datavector </summary>
        ///
        /// <returns> A deep copy of the datavector </summary>
        virtual std::unique_ptr<IDataVector> Clone() const override { return std::make_unique<ZeroDataVector>(*this); }

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override { return std::vector<double>(0); }
    };
}
}
