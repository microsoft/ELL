////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "IArchivable.h"
// stl
#include <cmath>
#include <iostream>
#include <vector>
#include <limits>

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

    /// <summary> Represents a constant reference to a vector, without a specified row or column orientation. </summary>
    ///
    /// <typeparam name="ElementType"> ElementType. </typeparam>
    template <typename ElementType>
    class UnorientedConstVectorReference
    {
    public:
        /// <summary> Constructs an instance of UnorientedConstVectorReference. </summary>
        ///
        /// <param name="pData"> [in,out] Pointer to the data. </param>
        /// <param name="size"> The size of the vector. </param>
        /// <param name="increment"> The vector increment. </param>
        UnorientedConstVectorReference(ElementType* pData, size_t size, size_t increment=1);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        inline ElementType operator[](size_t index) const;

        /// <summary> Gets the vector size. </summary>
        ///
        /// <returns> The vector size. </returns>
        size_t Size() const { return _size; }

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

        /// <summary> Swaps the contents of this with the contents of another UnorientedConstVectorReference. </summary>
        ///
        /// <param name="other"> [in,out] The other UnorientedConstVectorReference. </param>
        void Swap(UnorientedConstVectorReference<ElementType>& other);

        /// \name Math Functions
        /// @{

        /// <summary> Computes the 0-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm0() const;

        /// <summary> Computes the 1-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm1() const;

        /// <summary> Computes the 2-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm2() const;

        /// <summary> Computes the squared 2-norm of the vector </summary>
        ///
        /// <returns> The squared norm. </returns>
        ElementType Norm2Squared() const; 

        /// @}

        /// \name Utility Functions
        /// @{

        /// <summary> Applies a map to each vector element and sums the result. </summary>
        ///
        /// <typeparam name="MapperType"> A functor type of the mapper. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        ///
        /// <returns> The result of the operation. </returns>
        template <typename MapperType>
        ElementType Aggregate(MapperType mapper) const;

        /// <summary> Convert this vector into an array. </summary>
        ///
        /// <returns> An array that represents the data in this object. </returns>
        std::vector<ElementType> ToArray() const;

        /// @}

    protected:
        ElementType* _pData;
        size_t _size;
        size_t _increment;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class ConstVectorReference : public VectorBase<orientation>, public UnorientedConstVectorReference<ElementType>
    {
    public:
        using UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference;
        using UnorientedConstVectorReference<ElementType>::operator[];
        using UnorientedConstVectorReference<ElementType>::Size;
        using UnorientedConstVectorReference<ElementType>::GetDataPointer;
        using UnorientedConstVectorReference<ElementType>::GetIncrement;
        using UnorientedConstVectorReference<ElementType>::Norm0;
        using UnorientedConstVectorReference<ElementType>::Norm1;
        using UnorientedConstVectorReference<ElementType>::Norm2;
        using UnorientedConstVectorReference<ElementType>::Norm2Squared;

        /// <summary> Swaps the contents of this with the contents of another ConstVectorReference. </summary>
        ///
        /// <param name="other"> [in,out] The other ConstVectorReference. </param>
        void Swap(ConstVectorReference<ElementType, orientation>& other);

        /// \name Comparison Functions
        /// @{

        /// <summary> Check vector equality. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool IsEqual(ConstVectorReference<ElementType, orientation> other, ElementType tolerance = 1.0e-8) const;

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool operator==(const ConstVectorReference<ElementType, orientation>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are not considered equivalent. </returns>
        bool operator!=(const ConstVectorReference<ElementType, orientation>& other) const;

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a constant reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstVectorReference<ElementType, orientation> GetConstReference() const { return *this; }

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        ConstVectorReference<ElementType, orientation> GetSubVector(size_t offset, size_t size) const;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() const -> ConstVectorReference<ElementType, VectorBase<orientation>::transposeOrientation>
        {
            // STYLE intentional deviation from project style - long implementation should be in tcc file
            return ConstVectorReference<ElementType, VectorBase<orientation>::transposeOrientation>(_pData, _size, _increment);
        }

        /// @}

    protected:
        using UnorientedConstVectorReference<ElementType>::_pData;
        using UnorientedConstVectorReference<ElementType>::_size;
        using UnorientedConstVectorReference<ElementType>::_increment;
    };

    /// <summary> Prints a vector in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <param name="v"> The vector. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many tabs to print before the tensor. </param>
    /// <param name="maxElements"> (Optional) The maximal number of elements to print. </param>
    template <typename ElementType, VectorOrientation orientation>
    void Print(ConstVectorReference<ElementType, orientation> v, std::ostream& stream, size_t indent = 0, size_t maxElements = std::numeric_limits<size_t>::max());

    /// <summary> Prints a vector in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="v"> The const vector reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, VectorOrientation orientation>
    std::ostream& operator<<(std::ostream& stream, ConstVectorReference<ElementType, orientation> v);

    /// <summary> A class that represents a transformed constant vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    class TransformedConstVectorReference
    {
    public:
        /// <summary> Constructs an instance of TransformedConstVectorReference. </summary>
        ///
        /// <param name="vector"> The vector. </param>
        /// <param name="transform"> The transformation. </param>
        TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

        /// <summary> Gets the scalar. </summary>
        ///
        /// <returns> The scalar. </returns>
        const TransformationType GetTransformation() { return _transformation; }

        /// <summary> Gets the vector reference. </summary>
        ///
        /// <returns> The vector reference. </returns>
        ConstVectorReference<ElementType, orientation> GetVector() const { return _vector; }

    private:
        ConstVectorReference<ElementType, orientation> _vector;
        TransformationType _transformation;
    };

    /// <summary> Helper function that constructs a TransformedConstVectorReference from a vector and a transformation. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The vector. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> A TransformedConstVectorReference. </returns>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

    /// <summary> Multiplication operator for scalar and vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference that results from the operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise square operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise square-root operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square-root operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise absolute value operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise absolute value operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector);

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class VectorReference : public ConstVectorReference<ElementType, orientation>
    {
    public:
        using ConstVectorReference<ElementType, orientation>::ConstVectorReference;
        using ConstVectorReference<ElementType, orientation>::operator[];
        using ConstVectorReference<ElementType, orientation>::Size;
        using ConstVectorReference<ElementType, orientation>::GetDataPointer;
        using ConstVectorReference<ElementType, orientation>::GetIncrement;
        using ConstVectorReference<ElementType, orientation>::Norm0;
        using ConstVectorReference<ElementType, orientation>::Norm1;
        using ConstVectorReference<ElementType, orientation>::Norm2;
        using ConstVectorReference<ElementType, orientation>::Norm2Squared;
        using ConstVectorReference<ElementType, orientation>::IsEqual;
        using ConstVectorReference<ElementType, orientation>::operator==;
        using ConstVectorReference<ElementType, orientation>::operator!=;
        using ConstVectorReference<ElementType, orientation>::GetConstReference;
        using ConstVectorReference<ElementType, orientation>::GetSubVector;
        using ConstVectorReference<ElementType, orientation>::Transpose;

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> Reference to the specified element. </returns>
        inline ElementType& operator[](size_t index);

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() { return _pData; }

        /// <summary> Swaps the contents of this with the contents of another VectorReference. </summary>
        ///
        /// <param name="other"> [in,out] The other UnorientedConstVectorReference. </param>
        void Swap(VectorReference<ElementType, orientation>& other);

        /// \name  Content Manipulation Functions
        /// @{

        /// <summary> Copies values from another vector into this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void CopyFrom(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Copies values from a transformed vector into this vector. </summary>
        ///
        /// <param name="other"> The transformed vector. </param>
        template <typename TransformationType>
        void CopyFrom(TransformedConstVectorReference<ElementType, orientation, TransformationType> other);

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

        /// <summary> Applies a transfromation to each element of the vector. </summary>
        ///
        /// <typeparam name="TransformationType"> Tranformation type. </typeparam>
        /// <param name="transformation"> The transfromation. </param>
        template <typename TransformationType>
        void Transform(TransformationType transformation);

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A reference to this vector. </returns>
        VectorReference<ElementType, orientation> GetReference();

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        VectorReference<ElementType, orientation> GetSubVector(size_t offset, size_t size);

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() -> VectorReference<ElementType, VectorBase<orientation>::transposeOrientation>
        {
            return VectorReference<ElementType, VectorBase<orientation>::transposeOrientation>(_pData, _size, _increment);
        }

        /// @}

        /// \name Math Functions
        /// @{

        /// <summary> Adds another vector to this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void operator+=(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Adds a transformed vector to this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        template <typename TransformationType>
        void operator+=(TransformedConstVectorReference<ElementType, orientation, TransformationType> other);

        /// <summary> Subtracts another vector from this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void operator-=(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Adds a constant value to this vector. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator+=(ElementType value);

        /// <summary> Subtracts a constant value from this vector. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator-=(ElementType value);

        /// <summary> Multiplies this vector by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator*=(ElementType value);

        /// <summary> Divides each element of this vector by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator/=(ElementType value);

        /// @}

    protected:
        using ConstVectorReference<ElementType, orientation>::_pData;
        using ConstVectorReference<ElementType, orientation>::_size;
        using ConstVectorReference<ElementType, orientation>::_increment;
    };

    /// <summary> An algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation, row or colMajor. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class Vector : public VectorReference<ElementType, orientation>
    {
    public:
        using VectorReference<ElementType, orientation>::operator[];
        using VectorReference<ElementType, orientation>::Size;
        using VectorReference<ElementType, orientation>::GetDataPointer;
        using VectorReference<ElementType, orientation>::GetIncrement;
        using VectorReference<ElementType, orientation>::Norm0;
        using VectorReference<ElementType, orientation>::Norm1;
        using VectorReference<ElementType, orientation>::Norm2;
        using VectorReference<ElementType, orientation>::Norm2Squared;
        using VectorReference<ElementType, orientation>::IsEqual;
        using VectorReference<ElementType, orientation>::operator==;
        using VectorReference<ElementType, orientation>::operator!=;
        using VectorReference<ElementType, orientation>::GetConstReference;
        using VectorReference<ElementType, orientation>::GetSubVector;
        using VectorReference<ElementType, orientation>::GetReference;
        using VectorReference<ElementType, orientation>::Transpose;
        using VectorReference<ElementType, orientation>::CopyFrom;
        using VectorReference<ElementType, orientation>::Reset;
        using VectorReference<ElementType, orientation>::Fill;
        using VectorReference<ElementType, orientation>::Generate;
        using VectorReference<ElementType, orientation>::Transform;
        using VectorReference<ElementType, orientation>::operator+=;
        using VectorReference<ElementType, orientation>::operator-=;
        using VectorReference<ElementType, orientation>::operator*=;
        using VectorReference<ElementType, orientation>::operator/=;

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

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> A reference to this vector. </returns>
        Vector<ElementType, orientation>& operator=(Vector<ElementType, orientation> other);

        /// <summary> Resize the vector. This function possibly invalidates references to the old vector. </summary>
        ///
        /// <param name="size"> The new vector size. </param>
        void Resize(size_t size);

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

    /// <summary> A class that implements helper functions for archiving/unarchiving Vector instances. </summary>
    class VectorArchiver
    {
    public:
        /// <summary> Writes a vector to the archiver. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> The orientation, row or colMajor. </typeparam>
        /// <param name="tensor"> The vector to add to the archiver. </param>
        /// <param name="name"> The name of the vector value to add to the archiver. </param>
        /// <param name="archiver"> The `Archiver` to add the vector to </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Write(const Vector<ElementType, orientation>& vector, const std::string& name, utilities::Archiver& archiver);

        /// <summary> Reads a vector from the archiver. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> The orientation, row or colMajor. </typeparam>
        /// <param name="tensor"> The vector that will hold the result after it has been read from the archiver. </param>
        /// <param name="name"> The name of the vector value in the archiver. </param>
        /// <param name="archiver"> The `Archiver` to add the vector to </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Read(Vector<ElementType, orientation>& vector, const std::string& name, utilities::Unarchiver& archiver);
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