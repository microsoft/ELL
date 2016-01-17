// DoubleMatrix.h

#pragma once

#include "IMatrix.h"

#include <vector>
using std::vector;

namespace linear
{
    enum class MatrixStructure { column, row, columnSquare, rowSquare, rowSquareUptriangular, diagonal };

    /// Templated DoubleMatrix class with ElementType and StructureType template arguments
    ///
    template<MatrixStructure StructureType = MatrixStructure::row>
    class DoubleMatrix
    {};

    /// DoubleMatrix Base class
    class DoubleMatrixBase : public vector<double>, public IMatrix
    {
    public:

        /// Move constructor
        ///
        DoubleMatrixBase(DoubleMatrixBase&& other) = default;

        /// Deleted copy constructor
        ///
        DoubleMatrixBase(const DoubleMatrixBase& other) = delete;

        /// \returns The number of rows in the matrix
        ///
        virtual uint64 NumRows() const override;

        /// \returns The number of columns in the matrix
        ///
        virtual uint64 NumColumns() const override;

        /// Sets an entry in the matrix
        ///
        virtual void Set(uint64 i, uint64 j, double value = 1.0);

        /// Sets all of the matrix elements to zero
        ///
        void Reset();

        /// \returns A reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double& operator()(uint64 i, uint64 j) =0;

        /// \returns A constant reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double operator()(uint64 i, uint64 j) const =0;    

        /// Performs a general matrix vector product: y = alpha * M * x + beta * y
        ///
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// Performs a general vector matrix product: y = alpha * x * M + beta * y
        ///
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

        /// Prints the matrix to an output stream
        ///
        virtual void Print(ostream& os) const override;

    protected:
        DoubleMatrixBase(uint64 size, uint64 numRows, uint64 numColumns);

        uint64 _num_rows;
        uint64 _num_columns;

    private:
        double RowDot(uint64 i, const double* p_x) const;
        double ColumnDot(uint64 j, const double* p_x) const;
    };

    /// Templated DoubleMatrix class specialization for column-major matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::column> : public DoubleMatrixBase
    {
    public:
        
        /// Constructs a column major dense matrix
        ///
        DoubleMatrix(uint64 numRows, uint64 numColumns);

        /// \returns A reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double& operator()(uint64 i, uint64 j) override;

        /// \returns A constant reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double operator()(uint64 i, uint64 j) const override;
    };

    /// Templated DoubleMatrix class specialization for row-major matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::row> : public DoubleMatrixBase
    {
    public:

        /// Constructs a row major dense matrix
        ///
        DoubleMatrix(uint64 numRows, uint64 numColumns);

        /// \returns A reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double& operator()(uint64 i, uint64 j) override;

        /// \returns A constant reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double operator()(uint64 i, uint64 j) const override;
    };

    /// Templated DoubleMatrix class specialization for column-major square matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::columnSquare> : public DoubleMatrix<MatrixStructure::column>
    {
    public:
        /// Constructs a column major square dense matrix
        ///
        DoubleMatrix(uint64 dimension);
    };

    /// Templated DoubleMatrix class specialization for row-major square matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::rowSquare> : public DoubleMatrix<MatrixStructure::row>
    {
    public:
        /// Constructs a row major square dense matrix
        ///
        DoubleMatrix(uint64 dimension);
    };

    /// Templated DoubleMatrix class specialization for row-major square upper-triangular matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::rowSquareUptriangular> : public DoubleMatrixBase
    {
    public:
        /// Constructs a row major square upper-triangular dense matrix
        ///
        DoubleMatrix(uint64 dimension);

        /// Sets an entry in the matrix
        ///
        virtual void Set(uint64 i, uint64 j, double value = 1.0) override;

        /// \returns A reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double& operator()(uint64 i, uint64 j) override;

        /// \returns A constant reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double operator()(uint64 i, uint64 j) const override;

        /// Performs a general matrix vector product: y = alpha * M * x + beta * y
        ///
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// Performs a general vector matrix product: y = alpha * x * M + beta * y
        ///
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

    private:
        double _dummy = 0;
    };

    /// Templated DoubleMatrix class specialization for diagonal matrices
    ///
    template<>
    class DoubleMatrix<MatrixStructure::diagonal> : public DoubleMatrixBase
    {
    public:
        /// Constructs a row major square upper-triangular dense matrix
        ///
        DoubleMatrix(uint64 dimension);

        /// Sets an entry in the matrix
        ///
        virtual void Set(uint64 i, uint64 j, double value = 1.0) override;

        /// \returns A reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double& operator()(uint64 i, uint64 j) override;

        /// \returns A constant reference to an element of the matrix, specified by coordinates i,j
        ///
        virtual double operator()(uint64 i, uint64 j) const override;

        /// Performs a general matrix vector product: y = alpha * M * x + beta * y
        ///
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// Performs a general vector matrix product: y = alpha * x * M + beta * y
        ///
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

    private:
        double _dummy = 0;
    };
}
