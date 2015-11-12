// Base.cpp

#include "Base.h"

namespace linear
{
	ostream& operator<<(ostream& os, const Base& v)
	{
		v.print(os);
		return os;
	}
}