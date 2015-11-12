#pragma once

#include "types.h"

namespace linear
{
	class IndexValue
	{

	public:
		
		/// Ctor
		///
		IndexValue(uint index, double value);

		/// \returns The index
		///
		uint GetIndex() const;

		/// \return The value
		///
		double GetValue() const;

	private:
		uint _index;
		double _value;
	};
}