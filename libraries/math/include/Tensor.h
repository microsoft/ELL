////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Tensor.h (math)
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
    /// <summary> Base class for tensor reference classes. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Tensor element type. </typeparam>
    //template <typename ElementType>
    //class TensorReferenceBase
    //{
    //public:
    //    TensorReferenceBase(TensorReferenceBase&&) = default;
    //    TensorReferenceBase(const TensorReferenceBase&) = default;

    //protected:
    //    // protected ctor accessible only through derived classes
    //    TensorReferenceBase(ElementType* pData);

    //    // allow operations defined in the TensorOperations struct to access raw data vector
    //    friend struct TensorOperations;
    //    ElementType* GetDataPointer() { return _pData; }
    //    const ElementType* GetDataPointer() const { return _pData; }

    //    ElementType* _pData;
    //};

    /// <summary> Enum of possible matrix and vector orientations. </summary>
    enum class TensorOrientation { rowMajor, columnMajor };

    /// <summary> Helper class that flips orientation at compile time. </summary>
    ///
    /// <typeparam name="Orientation"> The original orientation. </typeparam>
    template<TensorOrientation Orientation>
    struct FlipOrientation 
    {};

    /// <summary> Helper class that flips rowMajor to columnMajor at compile time.  </summary>
    template<>
    struct FlipOrientation<TensorOrientation::rowMajor>
    {
        /// <summary> The flipped orientation value. </summary>
        static constexpr TensorOrientation value = TensorOrientation::columnMajor;
    };

    /// <summary> Helper class that flips columnMajor to rowMajor at compile time.  </summary>
    template<>
    struct FlipOrientation<TensorOrientation::columnMajor>
    {
        /// <summary> The flipped orientation value. </summary>
        static constexpr TensorOrientation value = TensorOrientation::rowMajor;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, TensorOrientation Orientation>
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
        /// <returns> A reference to this vector. </returns>
        const ConstVectorReference<ElementType, Orientation> GetReference() const;

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        /// <param name="strideMultiplier"> The stride multiplier of the sub-vector, defaults to 1. </param>
        ///
        /// <returns> The sub vector. </returns>
        const ConstVectorReference<ElementType, Orientation> GetSubVector(size_t offset, size_t size, size_t strideMultiplier=1) const;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        const ConstVectorReference<ElementType, FlipOrientation<Orientation>::value> Transpose() const;

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool operator==(const ConstVectorReference<ElementType, Orientation>& other) const;

    protected:
        // allow operations defined in the TensorOperations struct to access raw data vector
        friend struct TensorOperations;
        const ElementType* GetDataPointer() const { return _pData; }
        
        // protected ctor accessible only through derived classes
        ConstVectorReference(ElementType* pData, size_t size, size_t stride);

        // allow operations defined in the TensorOperations struct to access stride
        friend struct TensorOperations;
        size_t GetStride() const { return _stride; }

        ElementType* _pData;
        size_t _size;
        size_t _stride;

    private:
        template<typename MapperType>
        ElementType Aggregate(MapperType mapper) const;
    };

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientation. </typeparam>
    template <typename ElementType, TensorOrientation Orientation>
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
        /// <param name="strideMultiplier"> The stride multiplier of the sub-vector, defaults to 1. </param>
        ///
        /// <returns> The sub vector. </returns>
        VectorReference<ElementType, Orientation> GetSubVector(size_t offset, size_t size, size_t strideMultiplier=1);

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        VectorReference<ElementType, FlipOrientation<Orientation>::value> Transpose();

    protected:
        // allow operations defined in the TensorOperations struct to access raw data vector
        friend struct TensorOperations;
        ElementType* GetDataPointer() { return _pData; }
        
        // protected ctor accessible only through derived classes
        using ConstVectorReference<ElementType, Orientation>::ConstVectorReference;

    private:
        template<typename MapperType>
        void ForEach(MapperType mapper);
    };

    /// <summary> An algebraic vector. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> The orientationL rowMajor or colMajor. </typeparam>
    template <typename ElementType, TensorOrientation Orientation>
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

    typedef Vector<double, TensorOrientation::columnMajor> DoubleColumnVector;
    typedef Vector<double, TensorOrientation::rowMajor> DoubleRowVector;
    typedef ConstVectorReference<double, TensorOrientation::columnMajor> DoubleColumnConstVectorReference;
    typedef ConstVectorReference<double, TensorOrientation::rowMajor> DoubleRowConstVectorReference;

    typedef Vector<float, TensorOrientation::columnMajor> SingleColumnVector;
    typedef Vector<float, TensorOrientation::rowMajor> SingleRowVector;
    typedef ConstVectorReference<float, TensorOrientation::columnMajor> SingleColumnConstVectorReference;
    typedef ConstVectorReference<float, TensorOrientation::rowMajor> SingleRowConstVectorReference;
}

#include "../tcc/Tensor.tcc"
