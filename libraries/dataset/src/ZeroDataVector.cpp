////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ZeroDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZeroDataVector.h"

// stl
#include <stdexcept>

// utilities
#include "Exception.h"

namespace dataset
{
    linear::IndexValue ZeroDataVector::Iterator::Get() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
    }

    void ZeroDataVector::AppendEntry(uint64_t index, double value)
    {
        if (value != 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }
    }

    void ZeroDataVector::AddTo(double* p_other, double scalar) const
    {
    }

    double ZeroDataVector::Dot(const double* p_other) const
    {
        return 0.0;
    }

    void ZeroDataVector::Print(std::ostream& os) const
    {
    }
}
