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

    uint64_t AutoDataVector::Size() const
    {
        return uint64_t();
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

    void AutoDataVector::AppendEntry(uint64_t index, double value)
    {}

    uint64_t AutoDataVector::NumNonzeros() const
    {
        return uint64_t();
    }

    std::unique_ptr<IDataVector> AutoDataVector::Clone() const
    {
        return std::unique_ptr<IDataVector>();
    }

    std::vector<double> AutoDataVector::ToArray() const
    {
        return std::vector<double>();
    }

}
}