////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParametricReLUActivation.tcc (neural)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    ParametricReLUActivation<ElementType>::ParametricReLUActivation(TensorType alpha) : _alpha(std::move(alpha))
    {
        if (_alpha.GetDataPointer() == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "alpha tensor has null data field");
        }
    }

    template <typename ElementType>
    ElementType ParametricReLUActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet& index) const
    {
        return (( input > 0) ? input : _alpha(index) * input);
    }

    template <typename ElementType>
    void ParametricReLUActivation<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        math::TensorArchiver::Write(_alpha, "alpha", archiver);
    }

    template <typename ElementType>
    void ParametricReLUActivation<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        math::TensorArchiver::Read(_alpha, "alpha", archiver);
    }
}
}
}