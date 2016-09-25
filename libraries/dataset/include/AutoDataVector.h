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
    template<typename DefaultDataVectorType>
    class AutoDataVectorBase : public IDataVector
    {
    public:

        // TODO add iter ctor
        AutoDataVectorBase(const AutoDataVectorBase& vector) = delete;

        AutoDataVectorBase(AutoDataVectorBase&& vector) = default;

        AutoDataVectorBase(DefaultDataVectorType&& vector);
        
        /// <summary> Constructs a DenseDataVector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType>
        AutoDataVectorBase(IndexValueIteratorType indexValueIterator, typename linear::IsIndexValueIterator<IndexValueIteratorType> concept = 1);

        AutoDataVectorBase(std::initializer_list<linear::IndexValue> list);

        AutoDataVectorBase(std::initializer_list<double> list);

        virtual void AppendElement(size_t index, double value = 1.0) override;

        virtual IDataVector::Type GetType() const override { return IDataVector::Type::AutoDataVector; }

        IDataVector::Type GetInternalType() const { return _pInternal->GetType(); }

        virtual size_t Size() const override { return _pInternal->Size(); }

        virtual double Norm2() const override { return _pInternal->Norm2(); }

        virtual double Dot(const double * p_other) const override;

        virtual void AddTo(double * p_other, double scalar = 1.0) const override;

        virtual std::vector<double> ToArray() const override { return _pInternal->ToArray(); }

        template<typename ReturnType>
        ReturnType ToDataVector() const;

        virtual void Print(std::ostream & os) const override;

    private:

        void FindBestRepresentation(DefaultDataVectorType defaultDataVector);

        // members
        std::unique_ptr<IDataVector> _pInternal;
    };

    using AutoDataVector = AutoDataVectorBase<DoubleDataVector>;
}
}

#include "../tcc/AutoDataVector.tcc"