////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     DoubleMatrix.h (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IMatrix.h"

// stl
#include <cstdint>
#include <vector>
#include <iostream>

namespace linear
{
    /// <summary> Values that represent different matrix structures. </summary>
    enum class MatrixStructure { column, row, columnSquare, rowSquare, rowSquareUptriangular, diagonal };

    /// <summary>
    /// Templated DoubleMatrix class with StructureType template arguments.
    /// </summary>
    ///
    /// <typeparam name="StructureType"> Type of the matrix structure to use. </typeparam>
    template<MatrixStructure StructureType = MatrixStructure::row>
    class DoubleMatrix
    {};

    /// <summary> DoubleMatrix Base class. </summary>
    class DoubleMatrixBase : public std::vector<double>, public IMatrix
    {
    public:

        /// <summary> Move constructor. </summary>
        DoubleMatrixBase(DoubleMatrixBase&&) = default;

        /// <summary> Deleted copy constructor. </summary>
        DoubleMatrixBase(const DoubleMatrixBase&) = delete;

        /// <summary> Returns the number of rows in the matrix. </summary>
        ///
        /// <returns> The number of rows. </returns>
        virtual uint64_t NumRows() const override;

        /// <summary> Returns the number of columns in the matrix. </summary>
        ///
        /// <returns> The number of columns. </returns>
        virtual uint64_t NumColumns() const override;

        /// <summary> Sets an entry in the matrix. </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        /// <param name="value"> The value to set, or 1.0 by default. </param>
        virtual void Set(uint64_t i, uint64_t j, double value = 1.0);

        /// <summary> Sets all of the matrix elements to zero. </summary>
        void Reset();

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double& operator()(uint64_t i, uint64_t j) =0;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double operator()(uint64_t i, uint64_t j) const =0;    

        /// <summary> Performs a general matrix-vector product: y = alpha * M * x + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

        /// <summary> Prints the matrix to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    protected:
        DoubleMatrixBase(uint64_t size, uint64_t numRows, uint64_t numColumns);

        uint64_t _num_rows;
        uint64_t _num_columns;

    private:
        double RowDot(uint64_t i, const double* p_x) const;
        double ColumnDot(uint64_t j, const double* p_x) const;
    };

    /// <summary> Templated DoubleMatrix class specialization for column-major matrices. </summary>
    template<>
    class DoubleMatrix<MatrixStructure::column> : public DoubleMatrixBase
    {
    public:

        /// <summary> Constructs a column major dense matrix. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        DoubleMatrix(uint64_t numRows, uint64_t numColumns);

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double& operator()(uint64_t i, uint64_t j) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double operator()(uint64_t i, uint64_t j) const override;
    };

    /// <summary> Templated DoubleMatrix class specialization for row-major matrices. </summary>
    template<>
    class DoubleMatrix<MatrixStructure::row> : public DoubleMatrixBase
    {
    public:

        /// <summary> Constructs a row major dense matrix. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        DoubleMatrix(uint64_t numRows, uint64_t numColumns);

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double& operator()(uint64_t i, uint64_t j) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double operator()(uint64_t i, uint64_t j) const override;
    };

    /// <summary>
    /// Templated DoubleMatrix class specialization for column-major square matrices.
    /// </summary>
    template<>
    class DoubleMatrix<MatrixStructure::columnSquare> : public DoubleMatrix<MatrixStructure::column>
    {
    public:

        /// <summary> Constructs a column major square dense matrix. </summary>
        ///
        /// <param name="dimension"> The dimension. </param>
        DoubleMatrix(uint64_t dimension);
    };

    /// <summary> Templated DoubleMatrix class specialization for row-major square matrices. </summary>
    template<>
    class DoubleMatrix<MatrixStructure::rowSquare> : public DoubleMatrix<MatrixStructure::row>
    {
    public:

        /// <summary> Constructs a row major square dense matrix. </summary>
        ///
        /// <param name="dimension"> The dimension. </param>
        DoubleMatrix(uint64_t dimension);
    };

    /// <summary>
    /// Templated DoubleMatrix class specialization for row-major square upper-triangular matrices.
    /// </summary>
    template<>
    class DoubleMatrix<MatrixStructure::rowSquareUptriangular> : public DoubleMatrixBase
    {
    public:

        /// <summary> Constructs a row major square upper-triangular dense matrix. </summary>
        ///
        /// <param name="dimension"> The dimension. </param>
        DoubleMatrix(uint64_t dimension);

        /// <summary> Sets an entry in the matrix. </summary>
        ///
        /// <param name="i"> Zero-based index of the. </param>
        /// <param name="j"> The uint64_t to process. </param>
        /// <param name="value"> The value to set, 1.0 by default. </param>
        virtual void Set(uint64_t i, uint64_t j, double value = 1.0) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double& operator()(uint64_t i, uint64_t j) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double operator()(uint64_t i, uint64_t j) const override;

        /// <summary> Performs a general matrix-vector product: y = alpha * M * x + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

    private:
        double _dummy = 0;
    };

    /// <summary> Templated DoubleMatrix class specialization for diagonal matrices. </summary>
    template<>
    class DoubleMatrix<MatrixStructure::diagonal> : public DoubleMatrixBase
    {
    public:

        /// <summary> Constructs a row major square upper-triangular dense matrix. </summary>
        ///
        /// <param name="dimension"> The dimension. </param>
        DoubleMatrix(uint64_t dimension);

        /// <summary> Sets an entry in the matrix. </summary>
        ///
        /// <param name="i"> Zero-based index of the. </param>
        /// <param name="j"> The uint64_t to process. </param>
        /// <param name="value"> The value to set, 1.0 by default. </param>
        virtual void Set(uint64_t i, uint64_t j, double value = 1.0) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double& operator()(uint64_t i, uint64_t j) override;

        /// <summary> Returns a reference to a matrix element </summary>
        ///
        /// <param name="i"> Zero-based row index. </param>
        /// <param name="j"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the specified element. </returns>
        virtual double operator()(uint64_t i, uint64_t j) const override;

        /// <summary> Performs a general matrix-vector product: y = alpha * M * x + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gemv;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;
        using IMatrix::Gevm;

    private:
        double _dummy = 0;
    };
}
