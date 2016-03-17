////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     RowMatrix.tcc (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <stdexcept>
#include <random>
#include <cassert>

namespace linear
{
    template<typename DataVectorType>
    RowMatrix<DataVectorType>::Iterator::Iterator(const RowMatrix& table, uint64_t firstRow, uint64_t maxRow) : _table(table), _row(firstRow), _maxRow(maxRow)
    {}

    template<typename DataVectorType>
    bool RowMatrix<DataVectorType>::Iterator::IsValid() const
    {
        return _row < _maxRow;
    }

    template<typename DataVectorType>
    uint64_t RowMatrix<DataVectorType>::Iterator::NumIteratesLeft() const
    {
        return _maxRow - _row;
    }
    
    template<typename DataVectorType>
    void RowMatrix<DataVectorType>::Iterator::Next() 
    {
        ++_row;
    }
    
    template<typename DataVectorType>
    const DataVectorType& RowMatrix<DataVectorType>::Iterator::Get() const
    {
        assert(_row < _table.NumRows() && _row < _maxRow);
    
        return _table.GetRow(_row);
    }
    
    template<typename DataVectorType>
    uint64_t RowMatrix<DataVectorType>::NumRows() const
    {
        return _rows.size();
    }

    template<typename DataVectorType>
    uint64_t RowMatrix<DataVectorType>::NumColumns() const
    {
        return _num_columns;
    }

    template<typename DataVectorType>
    const DataVectorType& RowMatrix<DataVectorType>::GetRow(uint64_t index) const
    {
        return _rows[index];
    }

    template<typename DataVectorType>
    typename RowMatrix<DataVectorType>::Iterator RowMatrix<DataVectorType>::GetIterator(uint64_t firstRow, uint64_t numRows) const
    {
        uint64_t maxRow = firstRow + numRows;
        if(maxRow > NumRows() || numRows == 0)
        {
            maxRow = NumRows();
        }

        return Iterator(*this, firstRow, maxRow);
    }

    template<typename DataVectorType>
    void RowMatrix<DataVectorType>::PushBackRow(DataVectorType&& row)
    {
        uint64_t numColumns = row.Size();
        _rows.emplace_back(std::move(row));

        if(_num_columns < numColumns)
        {
            _num_columns = numColumns;
        }
    }

    template<typename DataVectorType>
    template<typename... Args >
    void RowMatrix<DataVectorType>::EmplaceBackRow(Args&&... args)
    {
        _rows.emplace_back(args...);

        uint64_t numColumns = _rows[_rows.size()-1].size();
        if(_num_columns < numColumns)
        {
            _num_columns = numColumns;
        }
    }

    template<typename DataVectorType>
    void RowMatrix<DataVectorType>::Gemv(const double* p_x, double* p_y, double alpha, double beta) const
    {
        int size = (int)NumRows(); // openmp doesn't like uint64_t

        if (alpha == 1.0)
        {
            if (beta == 0.0) // alpha == 1.0 && beta == 0.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = this->GetRow(i).Dot(p_x);
                }
            }
            else if (beta == 1.0) // alpha == 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] += this->GetRow(i).Dot(p_x);
                }
            }
            else // alpha == 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] = this->GetRow(i).Dot(p_x) + p_y[i] * beta;
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
                    p_y[i] = this->GetRow(i).Dot(p_x) * alpha;
                }
            }
            else if (beta == 1.0) // alpha != 1.0 && beta == 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] += this->GetRow(i).Dot(p_x) * alpha;
                }
            }
            else // alpha != 1.0 && beta != 0.0 && beta != 1.0
            {
                #pragma omp parallel for
                for (int i = 0; i < size; ++i)
                {
                    p_y[i] += this->GetRow(i).Dot(p_x) * alpha + p_y[i] * beta;
                }
            }
        }
    }

    template<typename DataVectorType>
    void RowMatrix<DataVectorType>::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
    {
        throw std::runtime_error("function not implemented");
    }

    template<typename DataVectorType>
    void RowMatrix<DataVectorType>::Print(std::ostream& os) const
    {
        for (uint64_t i = 0; i < NumRows(); ++i)
        {
            os << "Row " << i << "\t" << this->GetRow(i) << std::endl;
        }
    }

}

