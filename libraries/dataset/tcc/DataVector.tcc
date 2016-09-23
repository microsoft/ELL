////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//stl 
#include <cmath>

namespace emll
{
namespace dataset
{
    template<class DerivedType>
    double DataVectorBase<DerivedType>::Norm2() const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while(constIter.IsValid())
        {
            double value = constIter.Get().value;
            result += value * value;
            constIter.Next();
        }
        return std::sqrt(result);
    }

    template<class DerivedType>
    double DataVectorBase<DerivedType>::Dot(const double * p_other) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        double result = 0.0;
        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            result += indexValue.value * p_other[indexValue.index];
            constIter.Next();
        }
        return result;
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::AddTo(double * p_other, double scalar) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            p_other[indexValue.index] += scalar * indexValue.value;
            constIter.Next();
        }
    }

    template<class DerivedType>
    std::vector<double> DataVectorBase<DerivedType>::ToArray() const
    {
        std::vector<double> result(static_cast<const DerivedType*>(this)->Size());
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();

        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            result[indexValue.index] = indexValue.value;
            constIter.Next();
        }

        return result;
    }

    template<class DerivedType>
    void DataVectorBase<DerivedType>::Print(std::ostream & os) const
    {
        auto constIter = static_cast<const DerivedType*>(this)->GetIterator();
        if(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            os << indexValue.index << ":" << indexValue.value;
        }
        constIter.Next();
        
        while(constIter.IsValid())
        {
            auto indexValue = constIter.Get();
            os << '\t' << indexValue.index << ":" << indexValue.value;
            constIter.Next();
        }
    }
}
}