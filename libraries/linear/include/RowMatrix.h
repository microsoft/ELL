////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     RowMatrix.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RowMatrix.h"
#include "IMatrix.h"

#include <vector>

#include <iostream>

#include <random>

namespace linear
{
    /// <summary> Base class for RowMatrix. </summary>
    ///
    /// <typeparam name="DataVectorType"> Type of the data vector type. </typeparam>
    template<typename DataVectorType>
    class RowMatrix : public IMatrix
    {
    public:

        using IMatrix::Gemv;
        using IMatrix::Gevm;

        /// <summary> A read-only forward iterator of rows in the RowMatrix. </summary>
        class Iterator
        {
        public:

            /// <summary> Copy constructor. </summary>
            ///
            /// <param name="parameter1"> The first parameter. </param>
            Iterator(const Iterator&) = default;

            /// <summary> Move constructor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if valid, false if not. </returns>
            bool IsValid() const;

            /// <summary>
            /// Returns the number of iterates left in this iterator, including the current one.
            /// </summary>
            ///
            /// <returns> The total number of iterates left. </returns>
            uint64 NumIteratesLeft() const;

            /// <summary> Proceeds to the Next row. </summary>
            void Next();

            /// <summary> Returns a const reference to the row. </summary>
            ///
            /// <returns> A DataVectorType&amp; </returns>
            const DataVectorType& Get() const;

        private:

            // private ctor, can only be called from RowMatrix class
            Iterator(const RowMatrix& table, uint64 row, uint64 max_row);
            friend RowMatrix<DataVectorType>;

            // members
            const RowMatrix& _table;
            uint64 _row;
            uint64 _max_row;
        };

        /// <summary> The type of each row. </summary>
        using RowType = DataVectorType;

        /// <summary> Default constructor. </summary>
        RowMatrix() = default;

        /// <summary> Constructs an instance of RowMatrix. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        RowMatrix(RowMatrix<DataVectorType>&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="other"> The other. </param>
        RowMatrix(const RowMatrix<DataVectorType>& other) = delete;

        /// <summary> Default move assignment operator. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        ///
        /// <returns> A shallow copy of this RowMatrix. </returns>
        RowMatrix<DataVectorType>& operator=(RowMatrix<DataVectorType>&& other) = default;

        /// <summary> Deleted asignment operator. </summary>
        ///
        /// <param name="other"> The other. </param>
        ///
        /// <returns> A shallow copy of this RowMatrix. </returns>
        RowMatrix<DataVectorType>& operator=(const RowMatrix<DataVectorType>& other) = delete;

        /// <summary> Returns the number of rows in the matrix. </summary>
        ///
        /// <returns> The total number of rows. </returns>
        virtual uint64 NumRows() const override;

        /// <summary> Returns the number of columns in the matrix. </summary>
        ///
        /// <returns> The total number of columns. </returns>
        virtual uint64 NumColumns() const override;

        /// <summary> Returns a reference to a row. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The row. </returns>
        const RowType& GetRow(uint64 index) const;

        /// <summary> Returns an iterator that traverses the rows. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="size"> The size. </param>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator(uint64 row = 0, uint64 size = 0) const;

        /// <summary> Moves a row into the bottom of the matrix. </summary>
        ///
        /// <param name="row"> [in,out] The row. </param>
        void PushBackRow(DataVectorType&& row);

        /// <summary>
        /// Constructs a new row at the bottom of the matrix with given constructor arguments.
        /// </summary>
        ///
        /// <typeparam name="Args"> Type of the arguments. </typeparam>
        /// <param name="args"> Variable arguments providing [in,out] The arguments. </param>
        template<typename... Args >
        void EmplaceBackRow(Args&&... args);

        /// <summary> Randomly permutes the rows of the matrix. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        void RandPerm(std::default_random_engine& rng);

        /// <summary>
        /// Permutes the matrix so that the first numRows of the matrix are uniform, and the rest of the
        /// rows are arbitrary.
        /// </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="count"> Number of. </param>
        void RandPerm(std::default_random_engine& rng, uint64 count);

        /// <summary>
        /// Performs a general matrix std::vector product: y = alpha * M * x + beta * y.
        /// </summary>
        ///
        /// <param name="p_x"> The x coordinate. </param>
        /// <param name="p_y"> [in,out] If non-null, the y coordinate. </param>
        /// <param name="alpha"> The alpha. </param>
        /// <param name="beta"> The beta. </param>
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

        /// <summary>
        /// Performs a general std::vector matrix product: y = alpha * x * M + beta * y.
        /// </summary>
        ///
        /// <param name="p_x"> The x coordinate. </param>
        /// <param name="p_y"> [in,out] If non-null, the y coordinate. </param>
        /// <param name="alpha"> The alpha. </param>
        /// <param name="beta"> The beta. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    private:
        std::vector<DataVectorType> _rows;
        uint64 _num_columns = 0;
    };
}

#include "../tcc/RowMatrix.tcc"
