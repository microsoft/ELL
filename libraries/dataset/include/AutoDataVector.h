////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"

// stl
#include <initializer_list>

namespace emll
{
namespace dataset
{ 
    class AutoDataVector : public IDataVector
    {
    public:
        AutoDataVector(std::initializer_list<linear::IndexValue> list);

        AutoDataVector(std::initializer_list<double> list);

        virtual void AppendElement(size_t index, double value = 1.0) override;

        virtual IDataVector::Type GetType() const override { return IDataVector::Type::AutoDataVector; }

        virtual size_t Size() const override { return _pInternal->Size(); }

        virtual double Norm2() const override { return _pInternal->Norm2(); }

        virtual double Dot(const double * p_other) const override;

        virtual void AddTo(double * p_other, double scalar = 1.0) const override;

        virtual std::vector<double> ToArray() const override { return _pInternal->ToArray(); }

        virtual void Print(std::ostream & os) const override;

    private:
        std::unique_ptr<IDataVector> _pInternal;
    };
}
}