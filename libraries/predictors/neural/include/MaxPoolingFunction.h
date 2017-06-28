////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxPoolingFunction.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <limits>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the max function used in pooling layers. </summary>
    template <typename ElementType>
    class MaxPoolingFunction
    {
    public:
        /// <summary> Instantiates an instance of this function with all accumulation data cleared. </summary>
        MaxPoolingFunction();

        /// <summary> Adds another value for consideration. </summary>
        ///
        /// <param name="input"> The input value. </param>
        void Accumulate(ElementType value);

        /// <summary> Adds another value for consideration. </summary>
        ///
        /// <return> The average value from all accumulated values. </return>
        ElementType GetValue() const;
        
        /// <summary> Value to accumulate when on padding. </summary>
        ///
        /// <return> The value when accumulate is called on padding. </return>
        ElementType GetValueAtPadding() const { return -std::numeric_limits<ElementType>::max(); }

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "MaxPoolingFunction"; }

    private:
        ElementType _max;
    };
}
}
}

#include "../tcc/MaxPoolingFunction.tcc"