////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"

// utilities
#include "Exception.h"

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

        template<typename ReturnType>
        ReturnType ToDataVector() const;

        virtual void Print(std::ostream & os) const override;

    private:
        std::unique_ptr<IDataVector> _pInternal;
    };

    template<typename ReturnType>
    inline ReturnType AutoDataVector::ToDataVector() const
    {
        switch (_pInternal->GetType())
        {
        case IDataVector::Type::DoubleDataVector:
            return ReturnType(static_cast<DoubleDataVector*>(this)->GetIterator());

        case IDataVector::Type::FloatDataVector:
            return ReturnType(static_cast<FloatDataVector*>(this)->GetIterator());

        case IDataVector::Type::ShortDataVector:
            return ReturnType(static_cast<ShortDataVector*>(this)->GetIterator());

        case IDataVector::Type::ByteDataVector:
            return ReturnType(static_cast<ByteDataVector*>(this)->GetIterator());

        case IDataVector::Type::SparseDoubleDataVector:
            return ReturnType(static_cast<SparseDoubleDataVector*>(this)->GetIterator());

        case IDataVector::Type::SparseFloatDataVector:
            return ReturnType(static_cast<SparseFloatDataVector*>(this)->GetIterator());

        case IDataVector::Type::SparseShortDataVector:
            return ReturnType(static_cast<SparseShortDataVector*>(this)->GetIterator());

        case IDataVector::Type::SparseByteDataVector:
            return ReturnType(static_cast<SparseByteDataVector*>(this)->GetIterator());

        case IDataVector::Type::SparseBinaryDataVector:
            return ReturnType(static_cast<SparseBinaryDataVector*>(this)->GetIterator());

        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempted to cast unsupported data vector type");
        }


        return ReturnType();
    }
}
}