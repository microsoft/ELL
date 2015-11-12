// Base.h

#pragma once

#include "types.h"
#include <iostream>

using std::ostream;

namespace linear
{
	/// Base class for everything in this library
	///
	class Base
	{
	public:

		/// Virtual destructor
		///
		virtual ~Base() = default;

		/// Human readable printout to an output stream
		///
		virtual void Print(ostream& os) const = 0;
	};

	/// Generic operator<< that calls each class's Print() function
	///
	ostream& operator<<(ostream& os, const Base& v);
}