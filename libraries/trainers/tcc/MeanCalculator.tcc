////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanCalculator.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// data
#include "TransformedDataVector.h"

namespace ell
{
namespace trainers
{
    template<data::IterationPolicy policy, typename TransformationType>
    math::RowVector<double> CalculateTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

        math::RowVector<double> result;
        size_t count = 0;
        while (exampleIterator.IsValid())
        {
            const auto& x = exampleIterator.Get().GetDataVector();
            if (x.PrefixLength() > result.Size())
            {
                result.Resize(x.PrefixLength());
            }

            result += data::MakeTransformedDataVector<policy>(x, transformation);
            ++count;
            exampleIterator.Next();
        }

        double scale = 1.0 / count;
        result.Transform([scale](double x) {return scale*x; });

        return result;
    }

    template<typename TransformationType>
    math::RowVector<double> CalculateSparseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        return CalculateTransformedMean<data::IterationPolicy::skipZeros>(anyDataset, transformation);
    }

    template<typename TransformationType>
    math::RowVector<double> CalculateDenseTransformedMean(const data::AnyDataset& anyDataset, TransformationType transformation)
    {
        return CalculateTransformedMean<data::IterationPolicy::all>(anyDataset, transformation);
    }
}
}
