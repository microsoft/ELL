// SparseBinaryDatavector.h

#pragma once

#include "IDataVector.h"
#include "IntegerList.h"
#include "CompressedIntegerList.h"

namespace linear
{
	typedef uint64_t uint;

	/// Implements a sparse binary vector as an increasing list of the coordinates where the value is 1.0
	///
	template<typename IntegerListType>
	class SparseBinaryDatavectorBase : public IDataVector
	{
	public:

		/// A read-only forward iterator for the sparse binary vector.
		///
		class ConstIterator
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

			/// \returns The current index
			///
			uint GetIndex() const; // TODO: replace with IndexValue class as return type

			/// \returns The current value
			///
			double GetValue() const;

		private:

			// define typename to improve readability
			using index_iter_type = typename IntegerListType::ConstIterator;

			// private ctor, can only be called from SparseBinaryDatavectorBase class
			ConstIterator(const typename IntegerListType::ConstIterator& list_iterator);
			friend SparseBinaryDatavectorBase<IntegerListType>;

			// members
			index_iter_type _list_iterator;
		};

		/// Constructs an empty sparse binary vector
		///
		SparseBinaryDatavectorBase();

		/// Converting constructor
		///
		explicit SparseBinaryDatavectorBase(const IDataVector& other);

		/// Move constructor
		///
		SparseBinaryDatavectorBase(SparseBinaryDatavectorBase<IntegerListType>&& other) = default;

		/// Deleted copy constructor
		///
		SparseBinaryDatavectorBase(const SparseBinaryDatavectorBase<IntegerListType>& other) = delete;

		/// Sets the element at the given index to 1.0. Calls to this function must have a monotonically increasing argument. 
		/// The value argument must equal 1.0
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
		virtual double Norm2() const override;

		/// Performs (*p_other) += scalar * (*this), where other a dense vector
		///
		virtual void AddTo(double* p_other, double scalar = 1.0) const override;

		/// Computes the Dot product
		///
		virtual double Dot(const double* p_other) const override;

		/// \Returns a ConstIterator that traverses the non-zero entries of the sparse vector
		///
		ConstIterator GetConstIterator() const;

	private:
		IntegerListType _indices;
	};

	class  SparseBinaryDatavector : public SparseBinaryDatavectorBase<CompressedIntegerList>
	{
	public:
		using SparseBinaryDatavectorBase<CompressedIntegerList>::SparseBinaryDatavectorBase;

		/// \returns The type of the vector
		///
		virtual type GetType() const override;
	};

	class UncompressedSparseBinaryVector : public SparseBinaryDatavectorBase<IntegerList>
	{
	public:
		using SparseBinaryDatavectorBase<IntegerList>::SparseBinaryDatavectorBase;

		/// \returns The type of the vector
		///
		virtual type GetType() const override;
	};
}

