////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     LinearOperation.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearOperation.h"

// utilities
#include "StringFormat.h"

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

std::string LinearOperation::ToString(const std::string & sourceVariableName) const
{
    // assume that either _a != 0 or _b != 0

    if (_b == 0)
    {
        if (_a != 1)
        {
            return utilities::StringFormat("%f * %s", _a, sourceVariableName);
        }
        else
        {
            return sourceVariableName;
        }
    }
    else
    {
        if (_a == 0)
        {
            return std::to_string(_b);
        }
        else if (_a == 1)
        {
            return utilities::StringFormat("%s + %f", sourceVariableName, _b);
        }
        else
        {
            return utilities::StringFormat("%s * %f + %s", _a, sourceVariableName, _b);
        }
    }
}
