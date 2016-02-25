////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     BinaryClassificationEvaluator.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename ExampleIteratorType, typename PredictorType, typename LossFunctionType>
    void BinaryClassificationEvaluator::Evaluate(ExampleIteratorType& data_iter, const PredictorType& predictor, const LossFunctionType& loss_function)
    {
        
        Result current_eval; 
        double total_weight = 0;

        while(data_iter.IsValid())
        {
            const auto& example = data_iter.Get();
            double prediction = predictor.Predict(example);
            double loss = loss_function.Evaluate(prediction, example.GetLabel());

            total_weight += example.GetWeight();
            
            // count loss
            current_eval.loss += loss * example.GetWeight();

            // count prediction errors
            if(prediction * example.GetLabel() <= 0) current_eval.error += example.GetWeight();

            data_iter.Next();
        }

        current_eval.loss /= total_weight;
        current_eval.error /= total_weight;

        _evals.push_back(current_eval);
    }
}
