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

    /// <summary> Helper class that flips orientation at compile time. </summary>
    ///
    /// <typeparam name="Orientation"> The original orientation. </typeparam>
    template<VectorOrientation Orientation>
    struct FlipOrientation 
    {};

    /// <summary> Helper class that flips row to column at compile time.  </summary>
    template<>
    struct FlipOrientation<VectorOrientation::row>
    {
        /// <summary> The flipped orientation value. </summary>
        static constexpr VectorOrientation value = VectorOrientation::column;
    };

    /// <summary> Helper class that flips column to row at compile time.  </summary>
    template<>
    struct FlipOrientation<VectorOrientation::column>
    {
        /// <summary> The flipped orientation value. </summary>
        static constexpr VectorOrientation value = VectorOrientation::row;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class ConstVectorReference 
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

        /// <summary> Computes the 2-norm of the vector (not squared 2-norm). </summary>
        ///
        /// <returns> The 2-norm. </returns>
        ElementType Norm2() const; 

        /// <summary> Computes the 1-norm of the vector. </summary>
        ///
        /// <returns> The 1-norm. </returns>
        ElementType Norm1() const;

        /// <summary> Computes the 0-norm of the vector. </summary>
        ///
        /// <returns> The 0-norm. </returns>
        ElementType Norm0() const;

        /// <summary> Gets the minimal element in the vector. </summary>
        ///
        /// <returns> The minimal element. </returns>
        ElementType Min() const;

        /// <summary> Gets the maximal element in the vector. </summary>
        ///
        /// <returns> The maximal element. </returns>
        ElementType Max() const;

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
        ConstVectorReference<ElementType, FlipOrientation<Orientation>::value> Transpose() const;

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
        // allow operations defined in the Operations struct to access raw data vector
        friend struct Operations;
        const ElementType* GetDataPointer() const { return _pData; }
        
        // protected ctor accessible only through derived classes and friends
        friend class VectorConstructor;
        ConstVectorReference(ElementType* pData, size_t size, size_t increment);

        // allow operations defined in the Operations struct to access increment 
        friend struct Operations;
        size_t GetIncrement() const { return _increment ; }

        ElementType* _pData;
        size_t _size;
        size_t _increment ;

    private:
        template<typename MapperType>
        ElementType Aggregate(MapperType mapper) const;
    };

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation Orientation>
    class VectorReference : public ConstVectorReference<ElementType, Orientation>
    {
    public:
        /// <summary> Resets all the vector elements to zero. </summary>
        void Reset();

        /// <summary> Fills all of the vector elements with a given value. </summary>
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

        /// <summary> Addition assignment operator. </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void operator+=(ElementType scalar);

        /// <summary> Subtraction assignment operator. </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void operator-=(ElementType scalar);

        /// <summary> Multiplication assignment operator. </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void operator*=(ElementType scalar);

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
        VectorReference<ElementType, FlipOrientation<Orientation>::value> Transpose();
        using ConstVectorReference<ElementType, Orientation>::Transpose;

    protected:
        // allow operations defined in the Operations struct to access raw data vector
        friend struct Operations;
        ElementType* GetDataPointer() { return _pData; }
        
        // protected ctor accessible only through derived classes
        friend class VectorConstructor;
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

        /// <summary> Resets all the vector elements to zero. </summary>
        void Reset();

        /// <summary> Fills all of the vector elements with a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

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
