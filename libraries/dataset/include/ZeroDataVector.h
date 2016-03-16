////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     ZeroDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// types
#include "types.h"
#include "IndexValue.h"

namespace dataset
{
    /// <summary> ZeroDataVector Base class. </summary>
    class ZeroDataVector : public IDataVector
    {
    public:

        /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
        class Iterator : public types::IIndexValueIterator
        {
        public:

            /// <summary> Default copy ctor. </summary>
            ///
            /// <param name="parameter1"> The first parameter. </param>
            Iterator(const Iterator&) = default;

            /// <summary> Default move ctor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> \returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            types::IndexValue Get() const;

        private:
            // private ctor, can only be called from SparseDataVector class
            Iterator() = default;
            friend ZeroDataVector;
        };

        /// <summary> Default constructor. </summary>
        ZeroDataVector() = default;

        /// <summary> Converting constructor. </summary>
        ///
        /// <param name="other"> The other. </param>
        explicit ZeroDataVector(const IDataVector& other);

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        ZeroDataVector(ZeroDataVector&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        ZeroDataVector(const ZeroDataVector&) = delete;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;

        /// <summary> Sets an entry in the vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void PushBack(uint64 index, double value = 1.0) override;

        /// <summary> Deletes all of the vector content and sets its Size to zero, but does not deallocate
        /// its memory. </summary>
        virtual void Reset() override;

        /// <summary> \returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64 NumNonzeros() const override;

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

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

        /// <summary> \Returns a Iterator that points to the beginning of the datavector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;
    };
}
