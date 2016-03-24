////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     AsgdOptimizer.tcc (optimization)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cmath>
#include <cassert>

namespace optimization
{
    template<typename ExampleIteratorType, typename LossFunctionType>
    void AsgdOptimizer::Update(ExampleIteratorType& exampleIterator, uint64 numExamples, const LossFunctionType& lossFunction, double lambda)
    {
        // get references to the vector and biases
        auto& vLast = _lastPredictor.GetVector();
        auto& vAvg = _averagedPredictor.GetVector();

        double& bLast = _lastPredictor.GetBias();
        double& bAvg = _averagedPredictor.GetBias();

        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + numExamples;
        const double eta = 1.0 / lambda / T_prev;
        const double sigma = std::log(T_next) + 0.5 / T_next;

        vLast.AddTo(vAvg, sigma - std::log(T_prev) - 0.5 / T_prev);

        while(exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetLabel();
            double weight = example.GetWeight();
            const auto& dataVector = example.GetDataVector();

            // calculate the prediction 
            double alpha = T_prev / (t-1) * dataVector.Dot(vLast);

            // calculate the loss derivative
            double beta = weight * lossFunction.GetDerivative(alpha, label);

            // Update vLast and vAvg
            dataVector.AddTo(vLast, -eta*beta);
            dataVector.AddTo(vAvg, -eta*beta*(sigma - log(t) - 0.5/t));

            // move on
            exampleIterator.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        vLast.Scale(T_prev / T_next);
        vAvg.Scale(T_prev / T_next);
    }
}
