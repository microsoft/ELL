////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AutoDataVector.h"

namespace emll
{
namespace dataset
{ 



    void AutoDataVector::Print(std::ostream & os) const
    {}

    size_t AutoDataVector::Size() const
    {
        return size_t();
    }

    double AutoDataVector::Norm2() const
    {
        return 0.0;
    }

    void AutoDataVector::AddTo(double * p_other, double scalar) const
    {}

    double AutoDataVector::Dot(const double * p_other) const
    {
        return 0.0;
    }

    void AutoDataVector::AppendEntry(size_t index, double value)
    {}

    std::vector<double> AutoDataVector::ToArray() const
    {
        return std::vector<double>();
    }

}
}