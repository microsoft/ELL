// Shift.cpp

#include "Shift.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::to_string;

namespace mappings
{
	Shift::Shift() : Coordinatewise(Shift::Add)
	{
		_type = types::Shift;
	}

	double Shift::Add(double a, double b)
	{
		return a + b;
	}

}
