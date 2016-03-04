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
#include <stdexcept>
#include <cmath>
#include <cassert>

namespace optimization
{
    template<typename ExampleIteratorType, typename LossFunctionType>
    void AsgdOptimizer::Update(ExampleIteratorType& exampleIterator, const LossFunctionType& lossFunction, double lambda)
    {

        // get references to the vector and biases
        auto& v = _w;
        auto& v_avg = _predictor.GetVector();
        //double& b = _b;   // TODO (ofer) add learning of bias term
        //double& b_avg = _predictor.GetBias();

        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + exampleIterator.NumIteratesLeft();
        const double eta = 1.0 / lambda / T_prev;
        const double sigma = std::log(T_next) + 0.5 / T_next;

        v.AddTo(v_avg, sigma - std::log(T_prev) - 0.5 / T_prev);

        while(exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetLabel();
            double weight = example.GetWeight();

            // calculate the prediction 
            double alpha = T_prev / (t-1) * example.Dot(v);

            // calculate the loss derivative
            double beta = weight * lossFunction.GetDerivative(alpha, label);

            // Update v and v_avg
            example.AddTo(v, -eta*beta);
            example.AddTo(v_avg, -eta*beta*(sigma - log(t) - 0.5/t));

            // move on
            exampleIterator.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        v.Scale(T_prev / T_next);
        v_avg.Scale(T_prev / T_next);
    }
}
