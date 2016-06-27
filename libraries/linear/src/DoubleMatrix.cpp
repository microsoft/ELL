////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DoubleMatrix.cpp (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DoubleMatrix.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>

namespace linear
{

    void DoubleMatrixBase::Set(uint64_t i, uint64_t j, double value)
    {
        operator()(i,j) = value;
    }

    void DoubleMatrixBase::Gemv(const double* p_x, double* p_y, double alpha, double beta) const
    {
        int size = (int)NumRows(); // openmp doesn't like uint64_t

        if(alpha == 1.0)
        {
            if (beta == 0.0) // alpha == 1.0 && beta == 0.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = (double)RowDot(i, p_x);
                }
            }
            else if (beta == 1.0) // alpha == 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] += (double)RowDot(i, p_x);
                }
            }
            else // alpha == 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = (double)(RowDot(i, p_x) + p_y[i] * beta);
                }
            }
        }
        else 
        {
            if (beta == 0.0) // alpha != 1.0 && beta == 0.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = (double)(RowDot(i, p_x) * alpha);
                }
            }
            else if (beta == 1.0) // alpha != 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] += (double)(RowDot(i, p_x) * alpha);
                }
            }
            else // alpha != 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = (double)(RowDot(i, p_x) * alpha + p_y[i] * beta);
                }
            }
        }
    }

    void DoubleMatrixBase::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
    {
        int size = (int)NumColumns(); // openmp doesn't like uint64_t

        if (alpha == 1.0)
        {
            if (beta == 0.0) // alpha == 1.0 && beta == 0.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] = (double)ColumnDot(j, p_x);
                }
            }
            else if(beta == 1.0) // alpha == 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] += (double)ColumnDot(j, p_x);
                }
            }
            else // alpha == 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] = (double)(ColumnDot(j, p_x) + p_y[j] * beta);
                }
            }
        }
        else 
        {
            if (beta == 0.0) // alpha != 1.0 && beta == 0.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] = (double)(ColumnDot(j, p_x) * alpha);
                }
            }
            else if (beta == 1.0) // alpha != 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] += (double)(ColumnDot(j, p_x) * alpha);
                }
            }
            else // alpha != 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int j = 0; j < size; ++j)
                {
                    p_y[j] = (double)(ColumnDot(j, p_x) * alpha + p_y[j] * beta);
                }
            }
        }
    }

    void DoubleMatrixBase::Print(std::ostream& os) const
    {
        for(uint64_t i = 0; i < _numRows; ++i)
        {
            for(uint64_t j = 0; j < _numColumns; ++j)
            {
                os << this->operator()(i, j) << "\t";
            }
            os << std::endl;
        }
    }

    DoubleMatrixBase::DoubleMatrixBase(uint64_t size, uint64_t numRows, uint64_t numColumns)
        : std::vector<double>(size), _numRows(numRows), _numColumns(numColumns)
    {}

    double DoubleMatrixBase::RowDot(uint64_t i, const double* p_x) const
    {
        double value = 0;
        for (uint64_t j = 0; j < NumColumns(); ++j)
        {
            value += (double)(this->operator()(i, j) * p_x[j]);
        }
        return value;
    }

    double DoubleMatrixBase::ColumnDot(uint64_t j, const double* p_x) const
    {
        double value = 0;
        for (uint64_t i = 0; i < NumRows(); ++i)
        {
            value += (double)(this->operator()(i, j) * p_x[i]);
        }
        return value;
    }

    // MatrixStructure::column

    DoubleMatrix<MatrixStructure::column>::DoubleMatrix(uint64_t numRows, uint64_t numColumns) :
        DoubleMatrixBase(numRows*numColumns, numRows, numColumns)
    {}

    double& DoubleMatrix<MatrixStructure::column>::operator()(uint64_t i, uint64_t j)
    {
        assert(i < this->_numRows && j < this->_numColumns);
        return this->operator[](j * this->_numRows + i);
    }

    double DoubleMatrix<MatrixStructure::column>::operator()(uint64_t i, uint64_t j) const
    {
        assert(i < this->_numRows && j < this->_numColumns);
        return this->operator[](j * this->_numRows + i);
    }

    // MatrixStructure::row

    DoubleMatrix<MatrixStructure::row>::DoubleMatrix(uint64_t numRows, uint64_t numColumns) :
        DoubleMatrixBase(numRows*numColumns, numRows, numColumns)
    {}

    double& DoubleMatrix<MatrixStructure::row>::operator()(uint64_t i, uint64_t j)
    {
        assert(i < this->_numRows && j < this->_numColumns);
        return this->operator[](i * this->_numColumns + j);
    }

    double DoubleMatrix<MatrixStructure::row>::operator()(uint64_t i, uint64_t j) const
    {
        assert(i < this->_numRows && j < this->_numColumns);
        return this->operator[](i * this->_numColumns + j);
    }

    // MatrixStructure::columnSquare

    DoubleMatrix<MatrixStructure::columnSquare>::DoubleMatrix(uint64_t dimension) :
        DoubleMatrix<MatrixStructure::column>(dimension, dimension)
    {}

    // MatrixStructure::rowSquare

    DoubleMatrix<MatrixStructure::rowSquare>::DoubleMatrix(uint64_t dimension) :
        DoubleMatrix<MatrixStructure::row>(dimension, dimension)
    {}

    // MatrixStructure::rowSquareUptriangular

    DoubleMatrix<MatrixStructure::rowSquareUptriangular>::DoubleMatrix(uint64_t dimension) :
        DoubleMatrixBase(dimension*(dimension + 1) / 2, dimension, dimension)
    {}

    void DoubleMatrix<MatrixStructure::rowSquareUptriangular>::Set(uint64_t i, uint64_t j, double value)
    {
        if(i > j)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Cannot set element in the lower triangle of an upper triangular matrix");
        }

        DoubleMatrixBase::Set(i, j, value);
    }

    double& DoubleMatrix<MatrixStructure::rowSquareUptriangular>::operator()(uint64_t i, uint64_t j)
    {
        assert(i < this->_numRows && j < this->_numColumns);
        _dummy = 0;
        if(i > j) return _dummy;
        return this->operator[](i * this->_numColumns - (i + 1) * i / 2 + j);
    }

    double DoubleMatrix<MatrixStructure::rowSquareUptriangular>::operator()(uint64_t i, uint64_t j) const
    {
        assert(i < this->_numRows && j < this->_numColumns);
        if(i > j) return 0;
        return this->operator[](i * this->_numColumns - (i + 1) * i / 2 + j);
    }

    void DoubleMatrix<MatrixStructure::rowSquareUptriangular>::Gemv(const double* p_x, double* p_y, double alpha, double beta) const
    {
        // TODO (oferd) - replace the line below with optimized code for this type of matrix
        DoubleMatrixBase::Gemv(p_x, p_y, alpha, beta);
    }

    void DoubleMatrix<MatrixStructure::rowSquareUptriangular>::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
    {
        // TODO (oferd)- replace the line below with optimized code for this type of matrix
        DoubleMatrixBase::Gevm(p_x, p_y, alpha, beta);
    }

    // MatrixStructure::diagonal

    DoubleMatrix<MatrixStructure::diagonal>::DoubleMatrix(uint64_t dimension) :
        DoubleMatrixBase(dimension, dimension, dimension)
    {}

    void DoubleMatrix<MatrixStructure::diagonal>::Set(uint64_t i, uint64_t j, double value)
    {
        if(i != j)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Cannot set off-diagonal elements of a diagonal matrix");
        }

        DoubleMatrixBase::Set(i, j, value);
    }

    double& DoubleMatrix<MatrixStructure::diagonal>::operator()(uint64_t i, uint64_t j)
    {
        assert(i < this->_numRows && j < this->_numColumns);
        _dummy = 0;
        if(i != j) return _dummy;
        return this->operator[](i);
    }

    double DoubleMatrix<MatrixStructure::diagonal>::operator()(uint64_t i, uint64_t j) const
    {
        assert(i < this->_numRows && j < this->_numColumns);
        if(i != j) return 0;
        return this->operator[](i);
    }

    void DoubleMatrix<MatrixStructure::diagonal>::Gemv(const double* p_x, double* p_y, double alpha, double beta) const
    {
        // TODO (oferd) - replace the line below with optimized code for this type of matrix
        DoubleMatrixBase::Gemv(p_x, p_y, alpha, beta);
    }

    void DoubleMatrix<MatrixStructure::diagonal>::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
    {        
        // TODO (oferd) - replace the line below with optimized code for this type of matrix
        DoubleMatrixBase::Gevm(p_x, p_y, alpha, beta);
    }
}
