////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstVectorReference.h"
#include "VectorReference.h"

// stl
#include <cmath>
#include <iostream>
#include <vector>

namespace ell
{
namespace math
{
    /// <summary> An algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientationL row or colMajor. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class Vector : public VectorReference<ElementType, orientation>
    {
    public:
        /// <summary> Constructs an all-zeros vector of a given size. </summary>
        ///
        /// <param name="size"> The vector size. </param>
        Vector(size_t size = 0);

        /// <summary> Constructs a vector by copying a std::vector. </summary>
        ///
        /// <param name="data"> The std::vector to copy. </param>
        Vector(std::vector<ElementType> data);

        /// <summary> Constructs a vector from an initializer list. </summary>
        ///
        /// <param name="list"> The initalizer list. </param>
        Vector(std::initializer_list<ElementType> list);

        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The vector being moved. </param>
        Vector(Vector<ElementType, orientation>&& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The vector being copied. </param>
        Vector(const Vector<ElementType, orientation>& other);

        /// <summary> Resize the vector. This function possibly invalidates references to the old vector. </summary>
        ///
        /// <param name="size"> The new vector size. </param>
        void Resize(size_t size);

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> A reference to this vector. </returns>
        Vector<ElementType, orientation>& operator=(Vector<ElementType, orientation> other);

        /// <summary> Swaps the contents of this vector with the contents of another vector. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        void Swap(Vector<ElementType, orientation>& other);

    private:
        using ConstVectorReference<ElementType, orientation>::_pData;
        using ConstVectorReference<ElementType, orientation>::_size;

        // member variables
        std::vector<ElementType> _data;
    };

    //
    // friendly names
    //

    template <typename ElementType>
    using ColumnVector = Vector<ElementType, VectorOrientation::column>;

    template <typename ElementType>
    using RowVector = Vector<ElementType, VectorOrientation::row>;

    template <typename ElementType>
    using ColumnVectorReference = VectorReference<ElementType, VectorOrientation::column>;

    template <typename ElementType>
    using RowVectorReference = VectorReference<ElementType, VectorOrientation::row>;

    template <typename ElementType>
    using ColumnConstVectorReference = ConstVectorReference<ElementType, VectorOrientation::column>;

    template <typename ElementType>
    using RowConstVectorReference = ConstVectorReference<ElementType, VectorOrientation::row>;
}
}
#include "../tcc/Vector.tcc"
