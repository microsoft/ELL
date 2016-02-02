// BinaryClassificationEvaluator.h

#pragma once

// stl
#include <vector>
#include <iostream>

namespace utilities
{
    class BinaryClassificationEvaluator
    {
    public:

        struct Evaluation
        {
            double loss = 0;
            double error = 0;
        };
        
        BinaryClassificationEvaluator();

        template<typename ExampleIteratorType, typename PredictorType, typename LossFunctionType>
        void Evaluate(ExampleIteratorType& data_iter, const PredictorType& predictor, const LossFunctionType& loss_function);

        /// \returns The average weighted loss
        ///
        double GetLastLoss() const;

        /// \returns The weighted error rate
        ///
        double GetLastError() const;

        /// Prints losses and errors to an std::ostream
        void Print(std::ostream& os) const;

    private:
        std::vector<Evaluation> _evals;
    };

    std::ostream& operator<<(std::ostream& os, const BinaryClassificationEvaluator& evaluation);
}

#include "../tcc/BinaryClassificationEvaluator.tcc"
