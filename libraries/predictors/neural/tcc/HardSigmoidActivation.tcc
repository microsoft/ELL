////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationFunction.tcc (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType HardSigmoidActivation<ElementType>::Apply(const ElementType input) const
        {
            ElementType output = (static_cast<ElementType>(0.2) * input) + static_cast<ElementType>(0.5);
            return output < static_cast<ElementType>(0) ? static_cast<ElementType>(0) : (output > static_cast<ElementType>(1) ? static_cast<ElementType>(1) : output);
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> HardSigmoidActivation<ElementType>::Copy() const
        {
            return std::make_unique<HardSigmoidActivation<ElementType>>();
        }
    } // namespace neural
} // namespace predictors
} // namespace ell
