////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType LeakyReLUActivation<ElementType>::Apply(const ElementType input) const
        {
            return ((input > 0) ? input : _leakyFactor * input);
        }

        template <typename ElementType>
        void LeakyReLUActivation<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            archiver["leakyFactor"] << _leakyFactor;
        }

        template <typename ElementType>
        void LeakyReLUActivation<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            archiver["leakyFactor"] >> _leakyFactor;
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> LeakyReLUActivation<ElementType>::Copy() const
        {
            return std::make_unique<LeakyReLUActivation<ElementType>>(_leakyFactor);
        }
    } // namespace neural
} // namespace predictors
} // namespace ell
