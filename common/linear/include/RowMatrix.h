// RowMatrix.h

#pragma once

#include "RowMatrix.h"
#include "IMatrix.h"

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include <random>
using std::default_random_engine;

namespace linear
{
    /// Base class for RowMatrix
    ///
    template<typename DataVectorType>
    class RowMatrix : public IMatrix
    {
    public:

        using IMatrix::Gemv;
        using IMatrix::Gevm;

        /// A read-only forward iterator of rows in the RowMatrix.
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

            /// \returns the number of iterates left in this iterator, including the current one
            /// 
            uint NumIteratesLeft() const;

            /// Proceeds to the Next row
            ///
            void Next();

            /// \returns A const reference to the row
            ///
            const DataVectorType& GetValue() const;

        private:

            /// private ctor, can only be called from RowMatrix class
            Iterator(const RowMatrix& table, uint row, uint max_row);
            friend RowMatrix<DataVectorType>;

            // members
            const RowMatrix& _table;
            uint _row;
            uint _max_row;
        };

        /// The type of each row
        ///
        using RowType = DataVectorType;

        /// Default ctor
        ///
        RowMatrix() = default;

        /// Default move constructor
        ///
        RowMatrix(RowMatrix<DataVectorType>&& other) = default;

        /// Deleted copy constructor
        ///
        RowMatrix(const RowMatrix<DataVectorType>& other) = delete;

        /// \returns The number of rows in the matrix
        ///
        virtual uint NumRows() const override;

        /// \returns The number of columns in the matrix
        ///
        virtual uint NumColumns() const override;

        /// \returns A reference to a row
        ///
        const RowType& GetRow(uint index) const;

        /// \returns an iterator that traverses the rows
        ///
        Iterator GetIterator(uint row = 0, uint size = 0) const;
        
        /// Moves a row into the bottom of the matrix 
        ///
        void PushBackRow(DataVectorType&& row);

        /// Constructs a new row at the bottom of the matrix with given constructor arguments
        ///
        template<typename... Args >
        void EmplaceBackRow(Args&&... args);

        /// Randomly permutes the rows of the matrix
        ///
        void RandPerm(default_random_engine& rng);

        /// Permutes the matrix so that the first numRows of the matrix are uniform, and the rest of the rows are arbitrary
        /// 
        void RandPerm(default_random_engine& rng, uint count);

        /// Performs a general matrix vector product: y = alpha * M * x + beta * y
        ///
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

        /// Performs a general vector matrix product: y = alpha * x * M + beta * y
        ///
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

        /// Human readable printout to an output stream
        ///
        virtual void Print(ostream& os) const override;

    private:
        std::vector<DataVectorType> _rows;
        uint _num_columns = 0;
    };
}

#include "../tcc/RowMatrix.tcc"
