////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

namespace emll
{
namespace dataset
{ 
    class AutoDataVector : public IDataVector
    {
        virtual void Print(std::ostream & os) const override;

        virtual uint64_t Size() const override;

        virtual double Norm2() const override;

        virtual void AddTo(double * p_other, double scalar = 1.0) const override;

        virtual double Dot(const double * p_other) const override;

        virtual void AppendEntry(uint64_t index, double value = 1.0) override;

        virtual uint64_t NumNonzeros() const override;

        virtual std::vector<double> ToArray() const override;
    };
}
}