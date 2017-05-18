////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AbsoluteMeanTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

// math
#include "Vector.h"

namespace ell
{
    namespace trainers
    {
        /// <summary> a trainer that computes the mean of an unlabeled set of data vectors. </summary>
        class AbsoluteMeanTrainer : public ITrainer<math::RowVector<double>>
        {
        public:
            typedef math::RowVector<double> PredictorType;

            /// <summary> Updates the mean estimate with new data. </summary>
            ///
            /// <param name="anyDataset"> the dataset. </param>
            virtual void Update(const data::AnyDataset& anyDataset) override;

            /// <summary> Gets the mean. </summary>
            ///
            /// <returns> The mean. </returns>
            virtual const PredictorType& GetPredictor() const override;

        private:
            math::RowVector<double> _sum;
            double _count = 0;

            // this variable is mutable because it is evaluated in a lazy manner, when GetPredictor() is called
            mutable math::RowVector<double> _mean;
        };
    }
}