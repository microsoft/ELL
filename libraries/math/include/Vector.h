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
#include <functional>
#include <cmath>

namespace math
{
    /// <summary> Enum of possible vector orientations. </summary>
    enum class VectorOrientation { column, row };

    /// <summary> Base class for vectors. </summary>
    ///
    /// <typeparam name="Orientation"> The vector orientation. </typeparam>
    template<VectorOrientation Orientation>
    class VectorBase;

    /// <summary> Base class for row vectors. </summary>
    template<>
    class VectorBase<VectorOrientation::row>
    {
    protected:
        static constexpr VectorOrientation orientation = VectorOrientation::row;
        static constexpr VectorOrientation transposeOrientation = VectorOrientation::column;
    };

    /// <summary> Base class for column vectors. </summary>
    template<>
    class VectorBase<VectorOrientation::column>
    {
    protected:
        static constexpr VectorOrientation orientation = VectorOrientation::column;
        static constexpr VectorOrientation transposeOrientation = VectorOrientation::row;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class ConstVectorReference : public VectorBase<Orientation>
    {
    public:
        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        ElementType operator[] (size_t index) const;

        /// <summary> Gets the vector size. </summary>
        ///
        /// <returns> The vector size. </returns>
        size_t Size() const { return _size; }

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
        auto Transpose() const->ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;

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
        // protected ctor accessible only through derived classes and friends
        friend class VectorReferenceConstructor;
        friend class ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;
        ConstVectorReference(ElementType* pData, size_t size, size_t increment);

        // allow operations defined in the Operations struct to access raw data vector and increment
        friend struct NativeOperations;
        friend struct BlasOperations;

        ElementType* _pData;
        size_t _size;
        size_t _increment;
    };

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class VectorReference : public ConstVectorReference<ElementType, Orientation>
    {
    public:
        /// <summary> Sets all vector elements to zero. </summary>
        void Reset();

        /// <summary> Sets all vector elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary> Generates elements of the vector by repeatedly calling a generator function (such as a random number generator). </summary>
        /// <param name="generator"> The generator function. </param>
        void Generate(std::function<ElementType()> generator);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> Reference to the specified element. </returns>
        ElementType& operator[] (size_t index);
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
        auto Transpose()->VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;
        using ConstVectorReference<ElementType, Orientation>::Transpose;

    protected:
        // protected ctor accessible only through derived classes
        friend class VectorReferenceConstructor;
        friend class VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>;
        using ConstVectorReference<ElementType, Orientation>::ConstVectorReference;

    private:
        template<typename MapperType>
        void ForEach(MapperType mapper);
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

        /// <summary> Constructs a vector from an initializer list. </summary>
        ///
        /// <param name="list"> The initalizer list. </param>
        Vector(std::initializer_list<ElementType> list);

        /// <summary> Sets all vector elements to zero. </summary>
        void Reset();

        /// <summary> Sets all vector elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary> Generates elements of the vector by repeatedly calling a generator function (such as a random number generator). </summary>
        /// <param name="generator"> The generator function. </param>
        void Generate(std::function<ElementType()> generator);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> Reference to the specified element. </returns>
        ElementType& operator[] (size_t index);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        ElementType operator[] (size_t index) const;

    private:
        // member variables
        std::vector<ElementType> _data;
        using ConstVectorReference<ElementType, Orientation>::_pData; // TODO
    };

    //
    // friendly names
    //

    template<typename ElementType>
    using ColumnVector = Vector<ElementType, VectorOrientation::column>;

    template<typename ElementType>
    using RowVector = Vector<ElementType, VectorOrientation::row>;
}

#include "../tcc/Vector.tcc"
