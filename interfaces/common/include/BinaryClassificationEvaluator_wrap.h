////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryClassificationEvaluator_wrap.h (interfaces)
//  Authors:  Chuck Jacobs
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
    /// <summary> A binary classifier evaluator </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor type. </typeparam>
    /// <typeparam name="LossFunctionType"> Type of the loss function type. </typeparam>
    template<typename PredictorType, typename LossFunctionType>
    class BinaryClassificationEvaluator
    {
    public:
        typedef utilities::StlIterator<typename std::vector<dataset::SupervisedExample<dataset::IDataVector>>::const_iterator> ExampleIteratorType;

        /// <summary> The result of an evaluation. </summary>
        struct Result
        {
            double loss = 0;
            double error = 0;
        };
                
        BinaryClassificationEvaluator(const LossFunctionType& lossFunction);
        virtual void Evaluate(dataset::GenericRowIterator& dataIterator, const PredictorType& predictor) override;
        double GetLastLoss() const;
        double GetLastError() const;
        virtual void Print(std::ostream& os) const override;
    };
}
