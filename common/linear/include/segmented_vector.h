//// segmented_vector.h
//
//#pragma once
//
//#include "IDataVector.h"
//#include "DenseDatavector.h"
//
//// stl
//#include <vector>
//using std::vector;
//
//#define SEGMENT_SIZE 10000
//
//namespace linear
//{
//	/// Base class for segmented vectors
//	///
//	template<typename SegmentType = DenseDatavector>
//	class segmented_vector_base : public IDataVector
//	{
//	public:
//
//		/// Move constuctor
//		///
//		segmented_vector_base(segmented_vector_base&& other) = default;
//
//		/// Deleted copy constructor
//		///
//		segmented_vector_base(const segmented_vector_base&) = delete;
//
//		/// \returns The type of the vector
//		///
//		virtual type GetType() const override;
//
//		/// Calls a callback function for each non-zero entry in the vector, in order of increasing index
//		///
//		virtual	void foreach_nonzero(function<void(uint, double)> func, uint index_offset = 0) const override;
//
//		/// \returns The largest index of a non-zero entry plus one
//		///
//		virtual uint Size() const override;
//
//		/// Computes the vector squared 2-norm
//		///
//		virtual double Norm2() const override;
//
//		/// Performs (*p_other) += scalar * (*this), where other is a dense vector
//		///
//		virtual void AddTo(double* p_other, double scalar = 1.0) const override;
//		
//		/// Computes the vector Dot product
//		///
//		virtual double Dot(const double* p_other) const override;
//
//	protected:
//		segmented_vector_base(); // construct a derived class instead
//		
//		vector<SegmentType> _segments;
//	};
//
//	/// A segmented vector is a vector that is internally broken up stored in segments. At construction time, the Size of each segment is decided, and the dimension of the segmented vector is a multiple of the segment_size
//	///
//	template<typename SegmentType = DenseDatavector>
//	class segmented_vector : public segmented_vector_base<SegmentType>
//	{
//	public:
//
//		/// Constructor
//		///
//		segmented_vector();
//
//		/// Sets an entry in the vector
//		///
//		virtual void PushBack(uint index, double value = 1.0) override;
//	};
//
//}
//
//#include "../tcc/segmented_vector.tcc"
