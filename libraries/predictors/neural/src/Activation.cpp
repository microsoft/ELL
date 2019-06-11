////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Activation.cpp (neural)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HardSigmoidActivation.h"
#include "LeakyReLUActivation.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"
#include "TanhActivation.h"

using namespace ell::nodes;

namespace ell
{
namespace predictors
{
    namespace neural
    {

        //
        // Helper functions to convert predictors::neural activation functions into compilable node activation functions
        //
        template <typename ValueType>
        std::unique_ptr<ActivationFunction<ValueType>> GetNodeActivationFunction(const predictors::neural::Activation<ValueType>& f)
        {
            const predictors::neural::ActivationImpl<ValueType>* ptr = f.GetImpl();

            auto hardSigmoid = dynamic_cast<const predictors::neural::HardSigmoidActivation<ValueType>*>(ptr);
            if (hardSigmoid) return std::make_unique<HardSigmoidActivationFunction<ValueType>>();

            auto leakyReLU = dynamic_cast<const predictors::neural::LeakyReLUActivation<ValueType>*>(ptr);
            if (leakyReLU) return std::make_unique<LeakyReLUActivationFunction<ValueType>>(leakyReLU->GetLeakyFactor());

            auto sigmoid = dynamic_cast<const predictors::neural::SigmoidActivation<ValueType>*>(ptr);
            if (sigmoid) return std::make_unique<SigmoidActivationFunction<ValueType>>();

            auto relu = dynamic_cast<const predictors::neural::ReLUActivation<ValueType>*>(ptr);
            if (relu) return std::make_unique<ReLUActivationFunction<ValueType>>();

            auto tanh = dynamic_cast<const predictors::neural::TanhActivation<ValueType>*>(ptr);
            if (tanh) return std::make_unique<TanhActivationFunction<ValueType>>();

            auto prelu = dynamic_cast<const predictors::neural::ParametricReLUActivation<ValueType>*>(ptr);
            if (prelu)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetNodeActivationFunction cannot be used on ParametricReLUActivations");
            }

            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            std::string("GetNodeActivationFunction given a new Activation type it doesn't recognize: ") + typeid(*f.GetImpl()).name());
        }

        // Explicit instantiations
        template class Activation<float>;
        template class Activation<double>;

        template std::unique_ptr<ActivationFunction<float>> GetNodeActivationFunction<float>(const predictors::neural::Activation<float>& f);
        template std::unique_ptr<ActivationFunction<double>> GetNodeActivationFunction<double>(const predictors::neural::Activation<double>& f);
    } // namespace neural
} // namespace predictors
} // namespace ell
