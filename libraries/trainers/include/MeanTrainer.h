////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MeanTrainer.h (trainers)
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
        /// <summary> A trainer that computes the mean of an unlabeled set of data vectors. </summary>
        ///
        /// <typeparam name="TransformationType"> The type of transformation to apply before computing the mean. </typeparam>
        /// <typeparam name="policy"> The iteration policy to use with the transformation. </typeparam>
        template <typename TransformationType, data::IterationPolicy policy>
        class MeanTrainer : public ITrainer<math::RowVector<double>>
        {
        public:
            typedef math::RowVector<double> PredictorType;

            MeanTrainer() = default;

            /// <summary> Constructs an instance of MeanTrainer from a tranformation </summary>
            ///
            /// <param name="transformation"> The transformation. </param>
            MeanTrainer(TransformationType transformation);

            /// <summary> Updates the mean estimate with new data. </summary>
            ///
            /// <param name="anyDataset"> the dataset. </param>
            virtual void Update(const data::AnyDataset& anyDataset) override;

            /// <summary> Gets the mean transformed vector. </summary>
            ///
            /// <returns> The mean. </returns>
            virtual const PredictorType& GetPredictor() const override;

        private:
            TransformationType _transformation;
            math::RowVector<double> _sum;
            double _count = 0;

            // this variable is mutable because it is evaluated in a lazy manner, when GetPredictor() is called
            mutable math::RowVector<double> _mean;
        };

        /// <summary> Creates a mean trainer from a given sparse transformation. </summary>
        ///
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> The transformation. </param>
        ///
        /// <returns> A MeanTrainer.</returns>
        template <typename TransformationType>
        MeanTrainer<TransformationType, data::IterationPolicy::skipZeros> MakeSparseMeanTrainer(TransformationType transformation);

        /// <summary> Creates a mean trainer from a given dense transformation. </summary>
        ///
        /// <typeparam name="TransformationType"> The transformation type. </typeparam>
        /// <param name="transformation"> The transformation. </param>
        ///
        /// <returns> A MeanTrainer.</returns>
        template <typename TransformationType>
        MeanTrainer<TransformationType, data::IterationPolicy::all> MakeDenseMeanTrainer(TransformationType transformation);

        /// <summary> Creates a mean trainer with the identity tranformation. </summary>
        ///
        /// <returns> A MeanTrainer. </returns>
        auto MakeMeanTrainer() { return  MakeSparseMeanTrainer([](data::IndexValue x) {return x.value; }); }
    }
}

#include "../tcc/MeanTrainer.tcc"