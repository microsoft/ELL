////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>
#include <iostream>
#include <cmath>

namespace emll
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
    /// <typeparam name="Orientation"> The vector orientation. </typeparam>
    template <VectorOrientation Orientation>
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

    /// <summary> Represents a constant reference to a vector, without a specified row or column orientation. </summary>
    ///
    /// <typeparam name="ElementType"> ElementType. </typeparam>
    template<typename ElementType>
    class UnorientedConstVectorReference
    {
    public:
        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        ElementType operator[](size_t index) const;

        /// <summary> Gets the vector size. </summary>
        ///
        /// <returns> The vector size. </returns>
        size_t Size() const { return _size; }

        /// <summary> Convert this vector into an array. </summary>
        ///
        /// <returns> An array that represents the data in this object. </returns>
        std::vector<double> ToArray() const;

        /// <summary> Applies a map to each vector element and sums the result. </summary>
        ///
        /// <typeparam name="MapperType"> A functor type of the mapper. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        ///
        /// <returns> The result of the operation. </returns>
        template<typename MapperType>
        ElementType Aggregate(MapperType mapper) const;

    protected:
        UnorientedConstVectorReference(ElementType* pData, size_t size, size_t increment);

        ElementType* _pData;
        size_t _size;
        size_t _increment;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class ConstVectorReference : public VectorBase<Orientation>, public UnorientedConstVectorReference<ElementType>
    {
    public:
        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstVectorReference<ElementType, Orientation> GetReference() const;

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstVectorReference<ElementType, Orientation> GetConstantReference() const { return GetReference(); }

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        ConstVectorReference<ElementType, Orientation> GetSubVector(size_t offset, size_t size) const;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() const -> ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>
        {
            return ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>(_pData, _size, _increment);
        }

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool operator==(const ConstVectorReference<ElementType, Orientation>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are not considered equivalent. </returns>
        bool operator!=(const ConstVectorReference<ElementType, Orientation>& other) const;

    protected:
        using UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference;
        using UnorientedConstVectorReference<ElementType>::_pData;
        using UnorientedConstVectorReference<ElementType>::_size;
        using UnorientedConstVectorReference<ElementType>::_increment;

        template <typename T>
        friend class RectangularMatrixBase;

        friend class ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;
    };

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class VectorReference : public ConstVectorReference<ElementType, Orientation>
    {
    public:
        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Sets all vector elements to zero. </summary>
        void Reset();

        /// <summary> Sets all vector elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the vector by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template <typename GeneratorType>
        void Generate(GeneratorType generator);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> Reference to the specified element. </returns>
        ElementType& operator[](size_t index);
        using ConstVectorReference<ElementType, Orientation>::operator[];

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A reference to this vector. </returns>
        VectorReference<ElementType, Orientation> GetReference();
        using ConstVectorReference<ElementType, Orientation>::GetReference;

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        VectorReference<ElementType, Orientation> GetSubVector(size_t offset, size_t size);
        using ConstVectorReference<ElementType, Orientation>::GetSubVector;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() -> VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>
        {
            return VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>(_pData, _size, _increment);
        }
        using ConstVectorReference<ElementType, Orientation>::Transpose;

        /// <summary> Applies an operation to all items in this collection. </summary>
        ///
        /// <typeparam name="MapperType"> Type of the mapper functor. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        template <typename MapperType>
        void Transform(MapperType mapper);

    protected:
        using ConstVectorReference<ElementType, Orientation>::ConstVectorReference;
        using ConstVectorReference<ElementType, Orientation>::_pData;
        using ConstVectorReference<ElementType, Orientation>::_size;
        using ConstVectorReference<ElementType, Orientation>::_increment;

        template <typename T>
        friend class RectangularMatrixBase;

        friend class VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;
    };

    /// <summary> An algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientationL row or colMajor. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class Vector : public VectorReference<ElementType, Orientation>
    {
    public:
        /// <summary> Constructs an all-zeros vector of a given size. </summary>
        ///
        /// <param name="size"> The vector size. </param>
        Vector(size_t size);

        Vector(std::vector<ElementType> data);

        /// <summary> Constructs a vector from an initializer list. </summary>
        ///
        /// <param name="list"> The initalizer list. </param>
        Vector(std::initializer_list<ElementType> list);

    private:
        using ConstVectorReference<ElementType, Orientation>::_pData;
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
