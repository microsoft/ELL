////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AbsoluteMeanTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AbsoluteMeanTrainer.h"

namespace ell
{
    namespace trainers
    {
        void AbsoluteMeanTrainer::Update(const data::AnyDataset & anyDataset)
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

                _sum += x;
                ++_count;
                exampleIterator.Next();
            }
        }

        auto AbsoluteMeanTrainer::GetPredictor() const -> const PredictorType&
        {
            _mean.Resize(_sum.Size());
            _mean.Set((1 / _count) * _sum);
            return _mean;
        }
    }
}