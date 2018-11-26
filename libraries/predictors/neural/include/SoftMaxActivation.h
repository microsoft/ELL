////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxActivation.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <memory>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the softmax function. </summary>
        template <typename ElementType>
        class SoftMaxActivation
        {
        public:
            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
            void operator()(math::ColumnVectorReference<ElementType>& input) const;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
            void Apply(math::ColumnVectorReference<ElementType>& input) const;

            /// <summary> Typename used for serialization. </summary>
            /// Note: In the future, this will change to include the templated element type
            static std::string GetTypeName() { return "SoftMaxActivation"; }

            /// <summary> Archives this object. </summary>
            ///
            /// <param name="archiver"> The archiver. </param>
            void WriteToArchive(utilities::Archiver& /*archiver*/) const {};

            /// <summary> Unarchives this object. </summary>
            ///
            /// <param name="archiver"> The unarchiver. </param>
            void ReadFromArchive(utilities::Unarchiver& /*archiver*/){};
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <algorithm>
#include <limits>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        void SoftMaxActivation<ElementType>::Apply(math::ColumnVectorReference<ElementType>& input) const
        {
            ElementType maxVal = std::numeric_limits<ElementType>::lowest();
            for (size_t i = 0; i < input.Size(); ++i)
            {
                maxVal = std::max(maxVal, input[i]);
            }

            ElementType sum = 0;
            for (size_t i = 0; i < input.Size(); ++i)
            {
                const auto eulerVal = static_cast<ElementType>(std::exp(input[i] - maxVal));
                input[i] = eulerVal;
                sum += eulerVal;
            }

            const ElementType epsilon = static_cast<ElementType>(1e-7);
            if (sum < epsilon)
            {
                sum = 1.0;
            }

            input.Transform([sum](ElementType value) { return value / sum; });
        }

        template <typename ElementType>
        void SoftMaxActivation<ElementType>::operator()(math::ColumnVectorReference<ElementType>& input) const
        {
            return Apply(input);
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
