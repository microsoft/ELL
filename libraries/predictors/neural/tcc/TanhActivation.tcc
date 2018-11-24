////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TanhActivation.tcc (neural)
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
        ElementType TanhActivation<ElementType>::Apply(const ElementType input) const
        {
            return std::tanh(input);
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> TanhActivation<ElementType>::Copy() const
        {
            return std::make_unique<TanhActivation<ElementType>>();
        }
    } // namespace neural
} // namespace predictors
} // namespace ell
