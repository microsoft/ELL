////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorBase.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace math
{
    /// <summary> Enum of possible vector orientations. </summary>
    enum class VectorOrientation
    {
        column,
        row
    };

    /// <summary> Base class for vectors. </summary>
    ///
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    template <VectorOrientation orientation>
    class VectorBase;

    /// <summary> Base class for row vectors. </summary>
    template <>
    class VectorBase<VectorOrientation::row>
    {
    protected:
        static constexpr VectorOrientation orientation = VectorOrientation::row;
        static constexpr VectorOrientation transposeOrientation = VectorOrientation::column;
    };

    /// <summary> Base class for column vectors. </summary>
    template <>
    class VectorBase<VectorOrientation::column>
    {
    protected:
        static constexpr VectorOrientation orientation = VectorOrientation::column;
        static constexpr VectorOrientation transposeOrientation = VectorOrientation::row;
    };
}
}