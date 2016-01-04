// AsgdOptimizer.tcc

#include <stdexcept>
using std::runtime_error;

#include <cmath>
using std::log;

namespace optimization
{
    template<typename ExampleIteratorType, typename LossFunctionType>
    void AsgdOptimizer::Update(ExampleIteratorType& data_iter, const LossFunctionType& loss_function, double lambda)
    {

        // get references to the vector and biases
        DoubleVector& v = _w;
        DoubleVector& v_avg = _predictor.GetVector();
        double& b = _b;
        double& b_avg = _predictor.GetBias();

        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + data_iter.NumIteratesLeft();
        const double eta = 1.0 / lambda / T_prev;
        const double sigma = log(T_next) + 0.5 / T_next;

        v.AddTo(v_avg, sigma - log(T_prev) - 0.5 / T_prev);

        while(data_iter.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = data_iter.Get();
            double label = example.GetLabel();
            double weight = example.GetWeight();

            // calculate the prediction 
            double alpha = T_prev / (t-1) * example.Dot(v);

            // calculate the loss derivative
            double beta = weight * loss_function.GetDerivative(alpha, label);

            // Update v and v_avg
            example.AddTo(v, -eta*beta);
            example.AddTo(v_avg, -eta*beta*(sigma - log(t) - 0.5/t));

            // move on
            data_iter.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        v.Scale(T_prev / T_next);
        v_avg.Scale(T_prev / T_next);
    }
}
