// RowMatrix.tcc

#include <stdexcept>
using std::runtime_error;

#include<random>
using std::uniform_int_distribution;

#include <cassert>

using std::endl;
using std::swap;

namespace linear
{
	template<typename DatavectorType>
	RowMatrix<DatavectorType>::ConstIterator::ConstIterator(const RowMatrix& table, uint Row, uint max_row) : _table(table), _row(Row), _max_row(max_row)
	{}

	template<typename DatavectorType>
	bool RowMatrix<DatavectorType>::ConstIterator::IsValid() const
	{
		return _row < _max_row;
	}

	template<typename DatavectorType>
	uint RowMatrix<DatavectorType>::ConstIterator::NumIteratesLeft() const
	{
		return _max_row - _row;
	}
	
	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::ConstIterator::Next() 
	{
		++_row;
	}
	
	template<typename DatavectorType>
	const DatavectorType& RowMatrix<DatavectorType>::ConstIterator::GetValue() const
	{
		assert(_row < _table.NumRows() && _row < _max_row);
	
		return _table.GetRow(_row);
	}
	
	template<typename DatavectorType>
	uint64_t RowMatrix<DatavectorType>::NumRows() const
	{
		return _rows.size();
	}

	template<typename DatavectorType>
	uint RowMatrix<DatavectorType>::NumColumns() const
	{
		return _num_columns;
	}

	template<typename DatavectorType>
	const DatavectorType& RowMatrix<DatavectorType>::GetRow(uint64_t index) const
	{
		return _rows[index];
	}

	template<typename DatavectorType>
	typename RowMatrix<DatavectorType>::ConstIterator RowMatrix<DatavectorType>::GetIterator(uint Row, uint size) const
	{
		uint max_row = Row + size;
		if(max_row > NumRows() || size == 0)
		{
			max_row = NumRows();
		}

		return ConstIterator(*this, Row, max_row);
	}

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::PushBackRow(DatavectorType&& Row)
	{
		uint NumColumns = Row.Size();
		_rows.emplace_back(move(Row));

		if(_num_columns < NumColumns)
		{
			_num_columns = NumColumns;
		}
	}

	template<typename DatavectorType>
	template<typename... Args >
	void RowMatrix<DatavectorType>::EmplaceBackRow(Args&&... args)
	{
		_rows.emplace_back(args...);

		uint NumColumns = _rows[_rows.size()-1].size();
		if(_num_columns < NumColumns)
		{
			_num_columns = NumColumns;
		}
	}

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::RandPerm(default_random_engine& rng)
	{
		RandPerm(rng, NumRows());
	}

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::RandPerm(default_random_engine& rng, uint64_t count)
	{
		uint64_t max_row = NumRows()-1;

		for(uint64_t i = 0; i < count; ++i)
		{
			uniform_int_distribution<uint64_t> dist(i, max_row);
			uint64_t j = dist(rng);
			swap(_rows[i], _rows[j]);
		}
	}

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::Gemv(const double* p_x, double* p_y, double alpha, double beta) const
	{
		int size = (int)NumRows(); // openmp doesn't like uint

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

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
	{
		throw runtime_error("function not implemented");
	}

	template<typename DatavectorType>
	void RowMatrix<DatavectorType>::Print(ostream& os) const
	{
		for (uint i = 0; i < NumRows(); ++i)
		{
			os << "Row " << i << "\t" << this->GetRow(i) << endl;
		}
	}

}

