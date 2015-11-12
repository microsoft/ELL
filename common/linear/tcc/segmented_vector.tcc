// segmented_vector.tcc

namespace linear
{
	template<typename SegmentType>
	segmented_vector_base<SegmentType>::segmented_vector_base() 
	{
		_segments.emplace_back();
	}

	template<typename SegmentType>
	IDataVector::type segmented_vector_base<SegmentType>::GetType() const
	{
		return type::segmented;
	}

	//template<typename SegmentType>
	//void segmented_vector_base<SegmentType>::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const
	//{
	//	for(uint i = 0; i<_segments.Size(); ++i)
	//	{
	//		_segments[i].foreach_nonzero(func, index_offset + i * SEGMENT_SIZE);
	//	}
	//}

	template<typename SegmentType>
	uint segmented_vector_base<SegmentType>::size() const
	{
		return SEGMENT_SIZE * _segments.size();
	}

	template<typename SegmentType>
	double segmented_vector_base<SegmentType>::norm2() const
	{
		// TODO - parallelize
		double value = 0.0;
		for(const auto& segment : _segments)
		{
			value += segment.norm2();
		}
		return value;
	}

	template<typename SegmentType>
	void segmented_vector_base<SegmentType>::AddTo(double* p_other, double scalar) const
	{
		int size = (int)_segments.size(); // openmp doesn't like uint

        #pragma omp parallel for
		for(int i = 0; i<size; ++i)
		{
			_segments[i].AddTo(p_other + i *SEGMENT_SIZE, scalar);
		}
	}

	template<typename SegmentType>
	double segmented_vector_base<SegmentType>::Dot(const double* p_other) const
	{
		// TODO - parallelize
		double value = 0.0;
		for(uint i=0; i<_segments.size(); ++i)
		{
			value += _segments[i].Dot(p_other + i*SEGMENT_SIZE);
		}
		return value;
	}

	template<typename SegmentType>
	segmented_vector<SegmentType>::segmented_vector() : segmented_vector_base<SegmentType>()
	{}

	template<typename SegmentType>
	void segmented_vector<SegmentType>::PushBack(uint index, double value)
	{
		uint segment_index = index / SEGMENT_SIZE;
		uint internal_index = index % SEGMENT_SIZE;

		// make sure that the vector is long enough
		for(uint i = _segments.size()-1; i<=segment_index; ++i)
		{
			_segments.emplace_back();
		}

		_segments[segment_index].push_back(internal_index, value);
	}
}