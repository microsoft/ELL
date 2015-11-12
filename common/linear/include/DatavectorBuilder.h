// DatavectorBuilder.h

#pragma once

#include "IDataVector.h"

// stl
#include <memory>
using std::unique_ptr;
using std::move;

namespace linear
{
	/// DatavectorBuilder Base class
	///
	class DatavectorBuilder 
	{
	public:

		struct vector_stats
		{
			bool contains_nonbinary = false;
			bool contains_non_shorts = false;
			uint NumNonzeros = 0;
		};

		struct builder_stats
		{
			uint num_dense = 0;
			uint num_sparse = 0;
		};

		/// Constructs a vector builder
		///
		DatavectorBuilder();

		/// Sets an entry in the vector
		///
		void PushBack(uint index, double value);

		/// Calls a callback function for each non-zero entry in the vector, in order of increasing index
		///
		unique_ptr<IDataVector> GetVectorAndReset();

	private:
		unique_ptr<IDataVector> _up_vec;
		vector_stats _vector_stats;
		builder_stats _builder_stats;
	};
}

