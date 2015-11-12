// DenseDatavector.h

#pragma once

#include "IDataVector.h"
#include "IndexValue.h"

//#include <functional>

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#define DEFAULT_DENSE_VECTOR_CAPACITY 1000

namespace linear
{
	/// DenseDatavector Base class
	///
	template<typename ValueType>
	class DenseDatavector : public IDataVector
	{
	public:

		/// A read-only forward iterator for the sparse binary vector.
		///
		class ConstIterator // TODO: implement this type
		{
		public:

			/// Default copy ctor
			///
			ConstIterator(const ConstIterator&) = default;

			/// Default move ctor
			///
			ConstIterator(ConstIterator&&) = default;

			/// \returns True if the iterator is currently pointing to a valid iterate
			///
			bool IsValid() const;

			/// Proceeds to the Next iterate
			///
			void Next();

			/// \returns The current index-value pair
			///
			IndexValue GetValue() const;

		private:

			// define typenames to improve readability
			using stl_iter_type = typename vector<ValueType>::const_iterator;

			/// private ctor, can only be called from SparseDatavector class
			ConstIterator(const stl_iter_type& begin, const stl_iter_type& end);
			friend DenseDatavector<ValueType>;

			// members
			stl_iter_type _begin;
			stl_iter_type _end;
			uint _index;
		};


		/// Constructor
		///
		DenseDatavector();

		/// Converting constructor
		///
		explicit DenseDatavector(const IDataVector& other);

		/// Move constructor
		///
		DenseDatavector(DenseDatavector&& other) = default;

		/// Deleted copy constructor
		///
		DenseDatavector(const DenseDatavector&) = delete;

		/// Sets an entry in the vector
		///
		virtual void PushBack(uint index, double value = 1.0) override;

		/// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
		///
		virtual void Reset() override;

		/// Calls a callback function for each non-zero entry in the vector, in order of increasing index
		///
		//virtual	void foreach_nonzero(function<void(uint, double)> func, uint index_offset = 0) const override;

		/// \returns The largest index of a non-zero entry plus one
		///
		virtual uint Size() const override;

		/// \returns The number of non-zeros
		///
		virtual uint NumNonzeros() const override;

		/// Computes the vector squared 2-norm
		///
		virtual double norm2() const override;

		/// Performs (*p_other) += scalar * (*this), where other is a dense vector
		///
		virtual void AddTo(double* p_other, double scalar = 1.0) const override;
		
		/// Computes the Dot product
		///
		virtual double Dot(const double* p_other) const override;

	private:
		uint _num_nonzeros;
		vector<ValueType> _mem;
	};

	class FloatDatavector : public DenseDatavector<float> 
	{
	public:
		using DenseDatavector<float>::DenseDatavector;

		/// \returns The type of the vector
		///
		virtual type GetType() const override;
	};

	class DoubleDatavector : public DenseDatavector<double>
	{
	public:
		using DenseDatavector<double>::DenseDatavector;

		/// \returns The type of the vector
		///
		virtual type GetType() const override;
	};
}


