////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryClassificationEvaluator.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LogLoss.h"

namespace utilities
{
    template<typename PredictorType, typename LossFunctionType>
    BinaryClassificationEvaluator<PredictorType, LossFunctionType>::BinaryClassificationEvaluator()
    {}

    template<typename PredictorType, typename LossFunctionType>
    template<typename ExampleIteratorType>
    void BinaryClassificationEvaluator<PredictorType, LossFunctionType>::Evaluate(ExampleIteratorType& dataIterator, const PredictorType& predictor, const LossFunctionType& lossFunction)
    {
        Result currentEval; 
        double totalWeight = 0;

        while(dataIterator.IsValid())
        {
            const auto& example = dataIterator.Get();
            double prediction = predictor.Predict(example.GetDataVector());
            double loss = lossFunction.Evaluate(prediction, example.GetLabel());

            totalWeight += example.GetWeight();
            
            // count loss
            currentEval.loss += loss * example.GetWeight();

            // count prediction errors
            if(prediction * example.GetLabel() <= 0) currentEval.error += example.GetWeight();

            dataIterator.Next();
        }

        currentEval.loss /= totalWeight;
        currentEval.error /= totalWeight;

        _evals.push_back(currentEval);
    }

    template<typename PredictorType, typename LossFunctionType>
    void BinaryClassificationEvaluator<PredictorType, LossFunctionType>::Evaluate(utilities::AnyIterator<dataset::SupervisedExample>& dataIterator, const PredictorType& predictor, const LossFunctionType& lossFunction)
    {
        Evaluate<decltype(dataIterator)>(dataIterator, predictor, lossFunction);
    }

    template<typename PredictorType, typename LossFunctionType>
    double BinaryClassificationEvaluator<PredictorType, LossFunctionType>::GetLastLoss() const
    {
        if(_evals.size() == 0)
        {
            return 0;
        }

        return _evals[_evals.size()-1].loss;
    }

    template<typename PredictorType, typename LossFunctionType>
    double BinaryClassificationEvaluator<PredictorType, LossFunctionType>::GetLastError() const
    {
        if(_evals.size() == 0)
        {
            return 0;
        }

        return _evals[_evals.size()-1].error;
    }

    template<typename PredictorType, typename LossFunctionType>
    void BinaryClassificationEvaluator<PredictorType, LossFunctionType>::Print(std::ostream & os) const
    {
        os << "binary classification evaluation\nloss\terror\n";
        for(auto& e : _evals)
        {
            os << e.loss << '\t' << e.error << std::endl;
        }
    }

    template<typename PredictorType, typename LossFunctionType>
    std::ostream& operator<<(std::ostream & os, const BinaryClassificationEvaluator<PredictorType, LossFunctionType> & evaluator)
    {
        evaluator.Print(os);
        return os;
    }
}
