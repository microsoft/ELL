////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanPoolingFunction.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef> // size_t
#include <limits>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the average function used in pooling layers. </summary>
        template <typename ElementType>
        class MeanPoolingFunction
        {
        public:
            /// <summary> Instantiates an instance of this function with all accumulation data cleared. </summary>
            MeanPoolingFunction();

            /// <summary> Adds another value for consideration. </summary>
            ///
            /// <param name="input"> The input value. </param>
            void Accumulate(ElementType value);

            /// <summary> Returns the value of this function. </summary>
            ///
            /// <return> The average value from all accumulated values. </return>
            ElementType GetValue() const;

            /// <summary> Value to accumulate when on padding. </summary>
            ///
            /// <return> The value when accumulate is called on padding. </return>
            ElementType GetValueAtPadding() const { return 0; }

            /// <summary> Typename used for serialization. </summary>
            static std::string GetTypeName() { return "MeanPoolingFunction"; }

        private:
            ElementType _sum;
            size_t _numValues;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        MeanPoolingFunction<ElementType>::MeanPoolingFunction() :
            _sum(0),
            _numValues(0)
        {
        }

        template <typename ElementType>
        void MeanPoolingFunction<ElementType>::Accumulate(ElementType input)
        {
            _sum += input;
            _numValues++;
        }

        template <typename ElementType>
        ElementType MeanPoolingFunction<ElementType>::GetValue() const
        {
            return (_sum / (ElementType)_numValues);
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
