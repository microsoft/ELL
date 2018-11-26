////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Activation.tcc (neural)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <limits>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType ActivationImpl<ElementType>::operator()(const ElementType input) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        ElementType ActivationImpl<ElementType>::ApplyIndex(const ElementType input, const math::IntegerTriplet& /*index*/) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        Activation<ElementType>::Activation(std::unique_ptr<ActivationImpl<ElementType>>& impl) :
            _impl(std::move(impl))
        {}

        template <typename ElementType>
        Activation<ElementType>::Activation(ActivationImpl<ElementType>* impl) :
            _impl(impl)
        {
        }

        template <typename ElementType>
        Activation<ElementType>::Activation(const Activation<ElementType>& other) :
            _impl(std::move(other._impl->Copy()))
        {}

        template <typename ElementType>
        Activation<ElementType>& Activation<ElementType>::operator=(const Activation<ElementType>& other)
        {
            if (this != &other)
            {
                auto temp = other._impl->Copy();
                _impl.swap(temp);
            }
            return *this;
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::Apply(const ElementType input) const
        {
            return _impl->Apply(input);
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::operator()(const ElementType input) const
        {
            return _impl->Apply(input);
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const
        {
            return _impl->ApplyIndex(input, index);
        }

        template <typename ElementType>
        void Activation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
        {
            input.Transform([this](ElementType value) { return _impl->Apply(value); });
        }

        template <typename ElementType>
        void Activation<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            archiver["activation"] << _impl;
        }

        template <typename ElementType>
        void Activation<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            archiver["activation"] >> _impl;
        }
    } // namespace neural
} // namespace predictors
} // namespace ell
