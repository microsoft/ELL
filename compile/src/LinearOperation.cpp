// LinearOperation.cpp

#include "LinearOperation.h"

LinearOperation::LinearOperation(double a, double b) : _a(a), _b(b)
{}

LinearOperation LinearOperation::Compound(const LinearOperation & inputOperation) const
{
    return LinearOperation(_a * inputOperation._a, _a * inputOperation._b + _b);
}

bool LinearOperation::IsNull() const
{
    if(_a != 0.0)
    {
        return false;
    }
    if(_b != 0.0)
    {
        return false;
    }
    return true;
}
