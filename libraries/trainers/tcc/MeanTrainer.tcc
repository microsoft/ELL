////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformedDataVector.h"

namespace ell
{
namespace trainers
{
    template<typename TransformationType, data::IterationPolicy policy>
    MeanTrainer<TransformationType, policy>::MeanTrainer(TransformationType transformation) : _transformation(transformation)
    {
    }

    template <typename TransformationType, data::IterationPolicy policy>
    void MeanTrainer<TransformationType, policy>::Update(const data::AnyDataset & anyDataset)
    {
        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

        while (exampleIterator.IsValid())
        {
            const auto& x = exampleIterator.Get().GetDataVector();
            if (x.PrefixLength() > _sum.Size())
            {
                _sum.Resize(x.PrefixLength());
            }

            _sum += data::MakeTransformedDataVector<policy>(x, _transformation);
            ++_count;
            exampleIterator.Next();
        }
    }

    template <typename TransformationType, data::IterationPolicy policy>
    auto MeanTrainer<TransformationType, policy>::GetPredictor() const -> const PredictorType&
    {
        _mean.Resize(_sum.Size());
        _mean.Set((1 / _count) * _sum);
        return _mean;
    }

    template <typename TransformationType>
    MeanTrainer<TransformationType, data::IterationPolicy::skipZeros> MakeSparseMeanTrainer(TransformationType transformation)
    {
        return MeanTrainer<TransformationType, data::IterationPolicy::skipZeros>(transformation);
    }

    template <typename TransformationType>
    MeanTrainer<TransformationType, data::IterationPolicy::all> MakeDenseMeanTrainer(TransformationType transformation)
    {
        return MeanTrainer<TransformationType, data::IterationPolicy::all>(transformation);
    }
}
}