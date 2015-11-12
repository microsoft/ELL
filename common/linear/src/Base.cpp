// Base.cpp

#include "Base.h"

namespace linear
{
    ostream& operator<<(ostream& os, const Base& v)
    {
        v.Print(os);
        return os;
    }
}