////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxPoolingFunction.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <algorithm>
#include <limits>

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    MaxPoolingFunction<ElementType>::MaxPoolingFunction() : _max(std::numeric_limits<ElementType>::lowest())
    {
    }

    template <typename ElementType>
    void MaxPoolingFunction<ElementType>::Accumulate(ElementType input)
    {
        _max = std::max(_max, input);
    }

    template <typename ElementType>
    ElementType MaxPoolingFunction<ElementType>::GetValue() const
    {
        return _max;
    }
}
}
}
