////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryClassificationEvaluator.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SupervisedExample.h"
#include "RowDataset.h"
#include "LinearPredictor.h"

// stl
#include <vector>
#include <ostream>

namespace utilities
{
    /// <summary> Interace for a binary classification evaluator. </summary>
    template<typename PredictorType>
    class IBinaryClassificationEvaluator
    {
    public:
        typedef dataset::SupervisedExample<dataset::IDataVector> ExampleType;
//        typedef utilities::StlIterator<typename std::vector<ExampleType>::const_iterator> ExampleIteratorType;
        typedef dataset::GenericRowDataset::Iterator ExampleIteratorType;

        virtual void Evaluate(ExampleIteratorType& dataIterator, const PredictorType& predictor) = 0;

        virtual void Print(std::ostream& os) const = 0;
    };

    /// <summary> A binary classifier evaluator </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor type. </typeparam>
    /// <typeparam name="LossFunctionType"> Type of the loss function type. </typeparam>
    template<typename PredictorType, typename LossFunctionType>
    class BinaryClassificationEvaluator : public IBinaryClassificationEvaluator<PredictorType>
    {
    public:

        /// <summary> The result of an evaluation. </summary>
        struct Result
        {
            double loss = 0;
            double error = 0;
        };
                
        BinaryClassificationEvaluator(const LossFunctionType& lossFunction);

        /// <summary> Evaluates a binary classifier </summary>
        ///
        /// <param name="dataIterator"> [in,out] The data iterator. </param>
        /// <param name="predictor"> The predictor. </param>
        /// <param name="lossFunction"> The loss function. </param>
// ####
        virtual void Evaluate(typename IBinaryClassificationEvaluator<PredictorType>::ExampleIteratorType& dataIterator, const PredictorType& predictor) override;
//        virtual void Evaluate(dataset::GenericRowIterator& dataIterator, const PredictorType& predictor) override;

        /// <summary> Returns the most recent average weighted loss. </summary>
        ///
        /// <returns> The most recent loss. </returns>
        double GetLastLoss() const;

        /// <summary> Returns the most recent weighted error rate. </summary>
        ///
        /// <returns> The most recent error rate. </returns>
        double GetLastError() const;

        /// <summary> Prints losses and errors to an ostream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    private:
        LossFunctionType _lossFunction;
        std::vector<Result> _evals;
    };

    template <typename PredictorType, typename LossFunctionType>
    std::ostream& operator<<(std::ostream& os, const BinaryClassificationEvaluator<PredictorType, LossFunctionType>& evaluation);
}

#include "../tcc/BinaryClassificationEvaluator.tcc"
