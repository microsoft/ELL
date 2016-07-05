////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearOperation.cpp (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearOperation.h"

// utilities
#include "Format.h"
#include "Exception.h"

LinearOperation::LinearOperation(double a, double b) : _a(a), _b(b)
{}

LinearOperation::LinearOperation(layers::Coordinatewise::OperationType operationType, double coefficient)
{
    if(operationType == layers::Coordinatewise::OperationType::add)
    {
        _a = 1.0;
        _b = coefficient;
    }
    else if(operationType == layers::Coordinatewise::OperationType::multiply)
    {
        _a = coefficient;
        _b = 0;
    }
    else
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
    }
}

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
            return utilities::PrintFormat("% * %", _a, sourceVariableName);
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
            return utilities::PrintFormat("% + %", sourceVariableName, _b);
        }
        else
        {
            return utilities::PrintFormat("% * % + %", _a, sourceVariableName, _b);
        }
    }
}
