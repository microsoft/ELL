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

/*
Questions:

1. There are two reference classes: TensorReference and TensorConstReference. I couldn't figure out how to have a single class that can be "const" or not, and does the right thing.
2. Product is either between two Tensor, or between two TensorConstReference. No support for TensorReference. No support for combinations. All classes have a GetConstReference() member, so technically all of the tensor classes can become TensorConstReference and therefore the design is not limiting. The reason that products of raw `Tensor` are treated separately is the potential for a more efficient implementation without strides. 
3. Product of row vector and column vector produces a scalar, but instead of returning it, the Product function puts the result in the third argument (non-const reference to double). This is ugly, but the idea was that all of the Product functions have a similar signature.
*/

namespace math
{
    //
    // TensorBase and TensorReferenceBase classes
    // 

    /// <summary> Base class for tensors. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    template<typename ElementType>
    class TensorBase
    {
    public:
        TensorBase(TensorBase&&) = default;
        TensorBase(const TensorBase&) = delete;

        /// <summary> Resets all the tensor elements to zero. </summary>
        void Reset();

        /// <summary> Fills all of the tensor elements with a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary> Generates elements of the tensor by repeatedly calling a generator function (such as a random number generator). </summary>
        /// <param name="generator"> The generator function. </param>
        void Generate(std::function<ElementType()> generator);

    protected:
        // protected ctors accessible only through derived classes
        TensorBase(size_t size);
        TensorBase(std::vector<ElementType> data);

        template<class StlIteratorType>
        TensorBase(StlIteratorType begin, StlIteratorType end);

        // allow operations defined in the TensorOperations struct to access raw data vector
        friend struct TensorOperations;
        ElementType* GetDataPointer() { return _data.data(); }
        const ElementType* GetDataPointer() const { return _data.data(); }

        // member variables
        std::vector<ElementType> _data;
    };

    /// <summary> Base class for references to tensors. </summary>
    ///
    /// <typeparam name="ElementPointerType"> Type of the pointer to a tensor element. </typeparam>
    template<typename ElementPointerType>
    class TensorReferenceBase
    {
    public:
        TensorReferenceBase(TensorReferenceBase&&) = default;
        TensorReferenceBase(const TensorReferenceBase&) = default;

    protected:
        // protected ctor accessible only through derived classes
        TensorReferenceBase(ElementPointerType pData);

        // allow operations defined in the TensorOperations struct to access raw data vector
        friend struct TensorOperations;
        ElementPointerType GetDataPointer() { return _pData; }
        const ElementPointerType GetDataPointer() const { return _pData; }

        // member variables
        ElementPointerType _pData;
    };

    //
    // TensorDimensions classes
    // 

    /// <summary> Declaration of class used to represents the dimensions of the tensor. </summary>
    ///
    /// <typeparam name="TensorOrder"> The order of the tensor, the number of dimensions. </typeparam>
    template<size_t TensorOrder>
    class TensorDimensions {};

    /// <summary> Represents the dimension of a 1st order tensor, which is a vector. </summary>
    template<>
    class TensorDimensions<1>
    {
    public:

        /// <summary> Constructor. </summary>
        ///
        /// <param name="size"> The size of the 1st order tensor. </param>
        TensorDimensions(size_t size);

        /// <summary> Gets the size of the 1st order tensor, which is also the total number of elements. </summary>
        ///
        /// <returns> The tensor size. </returns>
        size_t Size() const { return _size; }

    protected:
        size_t _size;
    };

    /// <summary> Represents the dimensions of a 2nd order tensor, which is a matrix. </summary>
    template<>
    class TensorDimensions<2>
    {
    public:

        /// <summary> Constructor. </summary>
        ///
        /// <param name="numRows"> The number of rows. </param>
        /// <param name="numColumns"> The number of columns. </param>
        TensorDimensions(size_t numRows, size_t numColumns);

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _numRows; }

        /// <summary> Gets the number of of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _numColumns; }

        /// <summary> Gets the total number of elements. </summary>
        ///
        /// <returns> A number of elements. </returns>
        size_t Size() const { return _numRows * _numColumns; }

    protected:
        size_t _numRows;
        size_t _numColumns;
    };

    /// <summary> Represents the dimensions of a 3nd order tensor. </summary>
    template<>
    class TensorDimensions<3>
    {
    public:

        /// <summary> Constructor. </summary>
        ///
        /// <param name="size1"> The size of dimension 1. </param>
        /// <param name="size2"> The size of dimension 2. </param>
        /// <param name="size3"> The size of dimension 3. </param>
        TensorDimensions(size_t size1, size_t size2, size_t size3);

        /// <summary> Gets the size of dimension 1. </summary>
        ///
        /// <returns> The size of dimension 1. </returns>
        size_t GetSize1() const { return _size1; }

        /// <summary> Gets the size of dimension 2. </summary>
        ///
        /// <returns> The size of dimension  2. </returns>
        size_t GetSize2() const { return _size2; }

        /// <summary> Gets the size of dimension 3. </summary>
        ///
        /// <returns> The size of dimension 3. </returns>
        size_t GetSize3() const { return _size3; }

        /// <summary> Gets the total number of elements. </summary>
        ///
        /// <returns> Total number of elements. </returns>
        size_t Size() const { return _size1 * _size2 * _size3; }

    protected:
        size_t _size1;
        size_t _size2;
        size_t _size3;
    };

    //
    // Tensor, TensorReference, TensorConstReference classes
    // 

    /// <summary> Forward declaration of the tensor class (for subsequent specialization). </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="TensorOrder"> The order of the tensor: 1, 2, or 3. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, size_t TensorOrder, bool ColumnOrientation>
    class Tensor
    {};

    /// <summary> Forward declaration of the non-const tensor reference class (for subsequent specialization). </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="TensorOrder"> The order of the tensor: 1, 2, or 3. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, size_t TensorOrder, bool ColumnOrientation>
    class TensorReference
    {};

    /// <summary> Forward declaration of the const tensor reference class (for subsequent specialization). </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="TensorOrder"> The order of the tensor: 1, 2, or 3. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, size_t TensorOrder, bool ColumnOrientation>
    class TensorConstReference
    {};

    /// <summary> A non-const reference to a 1st order tensor. </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, bool ColumnOrientation>
    class TensorReference<ElementType, 1, ColumnOrientation> : public TensorReferenceBase<ElementType*>, public TensorDimensions<1>
    {
    public:

        /// <summary> Tensor indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Reference to the element at the given index. </returns>
        ElementType& operator() (size_t index) { return _pData[index*_stride]; }

        /// <summary> Tensor indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator() (size_t index) const { return _pData[index*_stride]; }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Reference to the element at the given index. </returns>
        ElementType& operator[] (size_t index) { return operator()(index); }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator[] (size_t index) const { return operator()(index); }

        /// <summary> Resets all the tensor elements to zero. </summary>
        void Reset();

        /// <summary> Fills all of the tensor elements with a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary> Generates elements of the tensor by repeatedly calling a generator function (such as a random number generator). </summary>
        /// <param name="generator"> The generator function. </param>
        void Generate(std::function<ElementType()> generator);

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> Zero-based offset to the first element of the sub-vector. </param>
        /// <param name="size"> The number of elements in the sub-vector. </param>
        /// <param name="strideMultiplier"> The stride multiplier. </param>
        ///
        /// <returns> Reference to the sub-vector. </returns>
        TensorReference<ElementType, 1, ColumnOrientation> GetSubVector(size_t offset, size_t size, size_t strideMultiplier = 1);

        /// <summary> Gets a reference to the transpose of the current vector. </summary>
        ///
        /// <returns> A reference to the transpose of the vector. </returns>
        TensorReference<ElementType, 1, !ColumnOrientation> Transpose();

        /// <summary> Gets constant reference to this sub-vector. </summary>
        ///
        /// <returns> The constant reference to this sub-vector. </returns>
        TensorConstReference<ElementType, 1, ColumnOrientation> GetConstReference();

    private:
        // private ctor can only be called by friends
        friend Tensor<ElementType, 1, ColumnOrientation>;
        friend TensorReference<ElementType, 1, !ColumnOrientation>;
        TensorReference(ElementType* pData, size_t size, size_t stride);

        friend TensorOperations;
        size_t _stride;
    };

    /// <summary> A const reference to a 1st order tensor. </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, bool ColumnOrientation>
    class TensorConstReference<ElementType, 1, ColumnOrientation> : public TensorReferenceBase<const ElementType*>, public TensorDimensions<1>
    {
    public:
        /// <summary> Tensor indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator() (size_t index) const { return _data[index*_stride]; }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator[] (size_t index) const { return operator()(index); }

        /// <summary> Gets a const reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> Zero-based offset to the first element of the sub-vector. </param>
        /// <param name="size"> The number of elements in the sub-vector. </param>
        /// <param name="strideMultiplier"> The stride multiplier. </param>
        ///
        /// <returns> Const reference to the sub-vector. </returns>
        TensorConstReference<ElementType, 1, ColumnOrientation> GetSubVector(size_t offset, size_t size, size_t strideMultiplier = 1);

        /// <summary> Gets a const reference to the transpose of the current vector. </summary>
        ///
        /// <returns> A const reference to the transpose of the vector. </returns>
        TensorConstReference<ElementType, 1, !ColumnOrientation> Transpose();

    private:
        // private ctor can only be called by friends
        friend Tensor<ElementType, 1, ColumnOrientation>;
        friend TensorReference<ElementType, 1, ColumnOrientation>;
        friend TensorConstReference<ElementType, 1, !ColumnOrientation>;
        TensorConstReference(const ElementType* pData, size_t size, size_t stride);

        friend TensorOperations;
        size_t _stride;
    };

    /// <summary> A 1st order tensor (vector). </summary>
    ///
    /// <typeparam name="ElementType"> Element type. </typeparam>
    /// <typeparam name="ColumnOrientation"> true if the tensor has column major orientation. </typeparam>
    template<typename ElementType, bool ColumnOrientation>
    class Tensor<ElementType, 1, ColumnOrientation> : public TensorBase<ElementType>, public TensorDimensions<1>
    {
    public:
        /// <summary> Constructs a 1st order tensor (vector) of a given size. </summary>
        ///
        /// <param name="size"> The size. </param>
        Tensor(size_t size);

        /// <summary> Constructs a 1st order tensor (vector) from a std::vector. </summary>
        ///
        /// <param name="data"> The vector. </param>
        Tensor(std::vector<ElementType> data);

        /// <summary> Constructs a 1st order tensor (vector) from an initializer list. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        Tensor(std::initializer_list<ElementType> list);

        /// <summary> Tensor indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Reference to the element at the given index. </returns>
        ElementType& operator() (size_t index) { return _data[index]; }

        /// <summary> Tensor indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator() (size_t index) const { return _data[index]; }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Reference to the element at the given index. </returns>
        ElementType& operator[] (size_t index) { return operator()(index); }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// 
        /// <returns> Copy of the element at the given index. </returns>
        ElementType operator[] (size_t index) const { return operator()(index); }

        /// <summary> Gets a non-constant reference to this sub-vector. </summary>
        ///
        /// <returns> A reference to this sub-vector. </returns>
        TensorReference<ElementType, 1, ColumnOrientation> GetReference();

        /// <summary> Gets constant reference to this sub-vector. </summary>
        ///
        /// <returns> A constant reference to this sub-vector. </returns>
        TensorConstReference<ElementType, 1, ColumnOrientation> GetConstReference() const;

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> true if the tensors are equal. </returns>
        bool operator==(const Tensor<ElementType, 1, ColumnOrientation>& other) const;
    };

    /// <summary> A struct that holds all of the binary tensor operations. </summary>
    struct TensorOperations
    {
        /// <summary> Calculates a vector dot product (between vectors in any orientation). </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <typeparam name="LeftOrientation"> Left vector orientation. </typeparam>
        /// <typeparam name="RightOrientation"> Right vector orientation. </typeparam>
        /// <param name="left"> The left vector, in any orientation. </param>
        /// <param name="right"> The right vector, in any orientation. </param>
        ///
        /// <returns> The dot product result. </returns>
        template<typename ElementType, bool LeftOrientation, bool RightOrientation>
        static ElementType Dot(const Tensor<ElementType, 1, LeftOrientation>& left, const Tensor<ElementType, 1, RightOrientation>& right);

        /// <summary> Calculates a vector dot product (between vectors in any orientation). </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <typeparam name="LeftOrientation"> Left vector orientation. </typeparam>
        /// <typeparam name="RightOrientation"> Right vector orientation. </typeparam>
        /// <param name="left"> The left vector, in any orientation. </param>
        /// <param name="right"> The right vector, in any orientation. </param>
        ///
        /// <returns> The dot product result. </returns>
        template<typename ElementType, bool LeftOrientation, bool RightOrientation>
        static ElementType Dot(const TensorConstReference<ElementType, 1, LeftOrientation>& left, const TensorConstReference<ElementType, 1, RightOrientation>& right);

        /// <summary> Calculates the product of a row vector with a column vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="left"> The left vector, in row orientation. </param>
        /// <param name="right"> The right vector, in column orientation. </param>
        /// <param name="result"> [out] The result. </param>
        template<typename ElementType>
        static void Product(const Tensor<ElementType, 1, false>& left, const Tensor<ElementType, 1, true>& right, ElementType& result);

        /// <summary> Calculates the product of a row vector with a column vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="left"> The left vector, in row orientation. </param>
        /// <param name="right"> The right vector, in column orientation. </param>
        /// <param name="result"> [out] The result. </param>
        template<typename ElementType>
        static void Product(const TensorConstReference<ElementType, 1, false>& left, const TensorConstReference<ElementType, 1, true>& right, ElementType& result);
    };

    //
    // typedefs
    // 
    typedef Tensor<double, 1, true> DoubleColumnVector;
    typedef Tensor<double, 1, false> DoubleRowVector;
    typedef TensorReference<double, 1, true> DoubleColumnVectorReference;
    typedef TensorReference<double, 1, false> DoubleRowVectorReference;
    typedef TensorConstReference<double, 1, true> DoubleColumnVectorConstReference;
    typedef TensorConstReference<double, 1, false> DoubleRowVectorConstReference;
}

#include "../tcc/Tensor.tcc"
