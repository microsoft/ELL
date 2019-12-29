////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/IArchivable.h>
#include <utilities/include/StlStridedIterator.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

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

    /// <summary> Helper class used to get the transpose orientation of a vector. </summary>
    ///
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    template <VectorOrientation orientation>
    struct TransposeVectorOrientation;

    /// <summary> Helper class used to get the transpose orientation of a vector. </summary>
    template <>
    struct TransposeVectorOrientation<VectorOrientation::row>
    {
        static constexpr VectorOrientation value = VectorOrientation::column;
    };

    /// <summary> Helper class used to get the transpose orientation of a vector. </summary>
    template <>
    struct TransposeVectorOrientation<VectorOrientation::column>
    {
        static constexpr VectorOrientation value = VectorOrientation::row;
    };

    /// <summary> Represents a constant reference to a vector, without a specified row or column orientation. </summary>
    ///
    /// <typeparam name="ElementType"> ElementType. </typeparam>
    template <typename ElementType>
    class UnorientedConstVectorBase
    {
    public:
        /// <summary> Constructs an instance of UnorientedConstVectorBase. </summary>
        ///
        /// <param name="pData"> Pointer to the data. </param>
        /// <param name="size"> The size of the vector. </param>
        /// <param name="increment"> The vector increment. </param>
        UnorientedConstVectorBase(const ElementType* pData, size_t size, size_t increment = 1);

        UnorientedConstVectorBase(const UnorientedConstVectorBase&) = default;

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        inline const ElementType& operator[](size_t index) const;

        /// <summary> Gets the vector size. </summary>
        ///
        /// <returns> The vector size. </returns>
        size_t Size() const { return _size; }

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetConstDataPointer() const { return _pData; }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

        /// <summary> Determines if this Vector is stored in contiguous memory. </summary>
        ///
        /// <returns> True if contiguous, false if not. </returns>
        bool IsContiguous() const { return _increment == 1; }

        /// <summary> Swaps the contents of this with the contents of another UnorientedConstVectorBase. </summary>
        ///
        /// <param name="other"> [in,out] The other UnorientedConstVectorBase. </param>
        void Swap(UnorientedConstVectorBase<ElementType>& other);

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

        /// <summary> Computes the infinity-norm of the vector </summary>
        ///
        /// <returns> The squared norm. </returns>
        ElementType NormInfinity() const;

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
        const ElementType* _pData;
        size_t _size;
        size_t _increment;
    };

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class ConstVectorReference : public UnorientedConstVectorBase<ElementType>
    {
    public:
        /// <summary> Constructs an instance of ConstVectorReference. </summary>
        ///
        /// <param name="pData"> Pointer to the data. </param>
        /// <param name="size"> The size of the vector. </param>
        /// <param name="increment"> The vector increment. </param>
        ConstVectorReference(const ElementType* pData, size_t size, size_t increment = 1);

        /// <summary> Swaps the contents of this with the contents of another ConstVectorReference. </summary>
        ///
        /// <param name="other"> [in,out] The other ConstVectorReference. </param>
        void Swap(ConstVectorReference<ElementType, orientation>& other);

        /// <summary>
        /// Visits elements of the vector by repeatedly calling a visitor function.
        /// </summary>
        ///
        /// <typeparam name="VisitorType"> Type of lambda or functor to use as a visitor. </typeparam>
        /// <param name="visitor"> The visitor function. </param>
        template <typename VisitorType>
        void Visit(VisitorType visitor) const;

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

        /// <summary> Equality operator for vectors with different orientation - always false. </summary>
        ///
        /// <returns> Always false. </returns>
        bool operator==(const ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>&) const { return false; }

        /// <summary> Inequality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are not considered equivalent. </returns>
        bool operator!=(const ConstVectorReference<ElementType, orientation>& other) const;

        /// <summary> Inequality operator for vectors with different orientation - always true. </summary>
        ///
        /// <returns> Always true. </returns>
        bool operator!=(const ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>&) const { return true; }

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
        auto Transpose() const -> ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>
        {
            // STYLE intentional deviation from project style - long implementation should be in the implementation region
            return ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>(this->GetConstDataPointer(), this->Size(), this->GetIncrement());
        }

        /// @}
    };

    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class VectorReference : public ConstVectorReference<ElementType, orientation>
    {
    public:
        /// <summary> Constructs an instance of VectorReference. </summary>
        ///
        /// <param name="pData"> Pointer to the data. </param>
        /// <param name="size"> The size of the vector. </param>
        /// <param name="increment"> The vector increment. </param>
        VectorReference(const ElementType* pData, size_t size, size_t increment = 1);

        using ConstVectorReference<ElementType, orientation>::operator[];
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
        ElementType* GetDataPointer() { return const_cast<ElementType*>(this->_pData); }

        /// <summary> Swaps the contents of this with the contents of another VectorReference. </summary>
        ///
        /// <param name="other"> [in,out] The other VectorReference. </param>
        void Swap(VectorReference<ElementType, orientation>& other);

        /// \name  Content Manipulation Functions
        /// @{

        /// <summary> Copies values from another vector into this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        template <typename OtherElementType>
        void CopyFrom(ConstVectorReference<OtherElementType, orientation> other);

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

        /// <summary>
        /// Applies an inplace transformation to each of the vector elements.
        /// </summary>
        ///
        /// <typeparam name="TransformationType"> Type of lambda or functor to use as a transformation. </typeparam>
        /// <param name="transformation"> The transformation function. </param>
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
        auto Transpose() -> VectorReference<ElementType, TransposeVectorOrientation<orientation>::value>
        {
            // STYLE intentional deviation from project style - long implementation should be in the implementation region
            return VectorReference<ElementType, TransposeVectorOrientation<orientation>::value>(this->GetDataPointer(), this->Size(), this->GetIncrement());
        }

        /// @}
    };

    /// <summary> An algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation, row or colMajor. </typeparam>
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
        /// <param name="list"> The initializer list. </param>
        Vector(std::initializer_list<ElementType> list);

        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The vector being moved. </param>
        Vector(Vector<ElementType, orientation>&& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The vector being copied. </param>
        Vector(const Vector<ElementType, orientation>& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The vector being copied. </param>
        Vector(const ConstVectorReference<ElementType, orientation>& other);

        /// <summary> Copies a vector of the opposite orientation. </summary>
        ///
        /// <param name="other"> The vector being copied. </param>
        Vector(const ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>& other);

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
        using ConstVectorReference<ElementType, orientation>::_increment;

        template <typename T, VectorOrientation o>
        friend auto begin(Vector<T, o>& vector) -> utilities::StlStridedIterator<typename std::vector<T>::iterator>;

        template <typename T, VectorOrientation o>
        friend auto end(Vector<T, o>& vector) -> utilities::StlStridedIterator<typename std::vector<T>::iterator>;

        template <typename T, VectorOrientation o>
        friend auto begin(const Vector<T, o>& vector) -> utilities::StlStridedIterator<typename std::vector<T>::const_iterator>;

        template <typename T, VectorOrientation o>
        friend auto end(const Vector<T, o>& vector) -> utilities::StlStridedIterator<typename std::vector<T>::const_iterator>;

        // member variables
        std::vector<ElementType> _data;
    };

    /// <summary> Get iterator to the beginning of a Vector </summary>
    ///
    /// <param name="vector"> The vector to get an iterator to. </param>
    ///
    /// <returns> A stl iterator to the beginning of the vector. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto begin(Vector<ElementType, orientation>& vector) -> utilities::StlStridedIterator<typename std::vector<ElementType>::iterator>;

    /// <summary> Get a const stl iterator to the beginning of a Vector </summary>
    ///
    /// <param name="vector"> The vector to get an iterator to. </param>
    ///
    /// <returns> A stl iterator to the beginning of the vector. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto begin(const Vector<ElementType, orientation>& vector) -> utilities::StlStridedIterator<typename std::vector<ElementType>::const_iterator>;

    /// <summary> Get iterator to the end of a Vector </summary>
    ///
    /// <param name="vector"> The vector to get an iterator to. </param>
    ///
    /// <returns> A stl iterator to the end of the vector. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto end(Vector<ElementType, orientation>& vector) -> utilities::StlStridedIterator<typename std::vector<ElementType>::iterator>;

    /// <summary> Get a const stl iterator to the end of a Vector </summary>
    ///
    /// <param name="vector"> The vector to get an iterator to. </param>
    ///
    /// <returns> A stl iterator to the end of the vector. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto end(const Vector<ElementType, orientation>& vector) -> utilities::StlStridedIterator<typename std::vector<ElementType>::const_iterator>;

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
    using ColumnVectorReference = VectorReference<ElementType, VectorOrientation::column>;

    template <typename ElementType>
    using ConstColumnVectorReference = ConstVectorReference<ElementType, VectorOrientation::column>;

    template <typename ElementType>
    using RowVector = Vector<ElementType, VectorOrientation::row>;

    template <typename ElementType>
    using RowVectorReference = VectorReference<ElementType, VectorOrientation::row>;

    template <typename ElementType>
    using ConstRowVectorReference = ConstVectorReference<ElementType, VectorOrientation::row>;
} // namespace math
} // namespace ell

#pragma region implementation

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>

namespace ell
{
namespace math
{
    //
    // UnorientedConstVectorBase
    //

    template <typename ElementType>
    UnorientedConstVectorBase<ElementType>::UnorientedConstVectorBase(const ElementType* pData, size_t size, size_t increment) :
        _pData(pData),
        _size(size),
        _increment(increment)
    {
    }

    template <typename ElementType>
    const ElementType& UnorientedConstVectorBase<ElementType>::operator[](size_t index) const
    {
        DEBUG_THROW(index >= _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return GetConstDataPointer()[index * _increment];
    }

    template <typename ElementType>
    void UnorientedConstVectorBase<ElementType>::Swap(UnorientedConstVectorBase<ElementType>& other)
    {
        std::swap(_pData, other._pData);
        std::swap(_size, other._size);
        std::swap(_increment, other._increment);
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::Norm0() const
    {
        return Aggregate([](ElementType x) { return x != 0 ? 1 : 0; });
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::Norm1() const
    {
        return Aggregate([](ElementType x) { return std::abs(x); });
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::Norm2() const
    {
        return std::sqrt(Norm2Squared());
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::Norm2Squared() const
    {
        return Aggregate([](ElementType x) { return x * x; });
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::NormInfinity() const
    {
        if (_size == 0)
        {
            return 0;
        }

        const ElementType* pData = GetConstDataPointer();
        const ElementType* pEnd = pData + _size * _increment;
        ElementType result = *pData;
        pData += _increment;

        while (pData < pEnd)
        {
            result = std::max(result, std::abs(*pData));
            pData += _increment;
        }

        return result;
    }

    template <typename ElementType>
    template <typename MapperType>
    ElementType UnorientedConstVectorBase<ElementType>::Aggregate(MapperType mapper) const
    {
        ElementType result = 0;
        const ElementType* current = GetConstDataPointer();
        const ElementType* end = current + _size * _increment;
        while (current < end)
        {
            result += mapper(*current);
            current += _increment;
        }
        return result;
    }

    template <typename ElementType>
    std::vector<ElementType> UnorientedConstVectorBase<ElementType>::ToArray() const
    {
        std::vector<ElementType> result(_size);

        const ElementType* pData = GetConstDataPointer();
        for (size_t i = 0; i < _size; ++i, pData += _increment)
            result[i] = *pData;

        return result;
    }

    //
    // ConstVectorReference
    //

    template <typename ElementType, VectorOrientation orientation>
    ConstVectorReference<ElementType, orientation>::ConstVectorReference(const ElementType* pData, size_t size, size_t increment) :
        UnorientedConstVectorBase<ElementType>(pData, size, increment)
    {
    }

    template <typename ElementType, VectorOrientation orientation>
    void ConstVectorReference<ElementType, orientation>::Swap(ConstVectorReference<ElementType, orientation>& other)
    {
        UnorientedConstVectorBase<ElementType>::Swap(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename VisitorType>
    void ConstVectorReference<ElementType, orientation>::Visit(VisitorType visitor) const
    {
        const ElementType* data = this->GetConstDataPointer();
        const ElementType* end = data + this->Size() * this->GetIncrement();

        while (data < end)
        {
            visitor(*data);
            data += this->GetIncrement();
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::IsEqual(ConstVectorReference<ElementType, orientation> other, ElementType tolerance) const
    {
        if (this->Size() != other.Size())
        {
            return false;
        }

        const ElementType* pThis = this->GetConstDataPointer();
        const ElementType* pThisEnd = pThis + this->Size() * this->GetIncrement();
        const ElementType* pOther = other.GetConstDataPointer();

        while (pThis < pThisEnd)
        {
            auto diff = (*pThis) - (*pOther);

            if (diff > tolerance || -diff > tolerance)
            {
                return false;
            }
            pThis += this->GetIncrement();
            pOther += other.GetIncrement();
        }
        return true;
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::operator==(const ConstVectorReference<ElementType, orientation>& other) const
    {
        return IsEqual(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::operator!=(const ConstVectorReference<ElementType, orientation>& other) const
    {
        return !(*this == other);
    }

    template <typename ElementType, VectorOrientation orientation>
    ConstVectorReference<ElementType, orientation> ConstVectorReference<ElementType, orientation>::GetSubVector(size_t offset, size_t size) const
    {
        DEBUG_THROW(offset + size > this->Size(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return ConstVectorReference<ElementType, orientation>(this->GetConstDataPointer() + offset * this->GetIncrement(), size, this->GetIncrement());
    }

    //
    // VectorReference
    //

    template <typename ElementType, VectorOrientation orientation>
    VectorReference<ElementType, orientation>::VectorReference(const ElementType* pData, size_t size, size_t increment) :
        ConstVectorReference<ElementType, orientation>(pData, size, increment)
    {
    }

    template <typename ElementType, VectorOrientation orientation>
    ElementType& VectorReference<ElementType, orientation>::operator[](size_t index)
    {
        DEBUG_THROW(index >= this->Size(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return GetDataPointer()[index * this->GetIncrement()];
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::Swap(VectorReference<ElementType, orientation>& other)
    {
        ConstVectorReference<ElementType, orientation>::Swap(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename OtherElementType>
    void VectorReference<ElementType, orientation>::CopyFrom(ConstVectorReference<OtherElementType, orientation> other)
    {
        if (this->Size() != other.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const OtherElementType* pOtherData = other.GetConstDataPointer();
        const size_t otherIncrement = other.GetIncrement();
        const OtherElementType* pOtherEnd = pOtherData + otherIncrement * other.Size();

        if (this->GetIncrement() == 1 && otherIncrement == 1)
        {
            while (pOtherData < pOtherEnd)
            {
                (*pData) = static_cast<ElementType>(*pOtherData);
                ++pData;
                ++pOtherData;
            }
        }
        else
        {
            while (pOtherData < pOtherEnd)
            {
                (*pData) = static_cast<ElementType>(*pOtherData);
                pData += this->GetIncrement();
                pOtherData += otherIncrement;
            }
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::Reset()
    {
        Fill(0);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::Fill(ElementType value)
    {
        ElementType* data = GetDataPointer();
        ElementType* end = data + this->Size() * this->GetIncrement();

        if (this->IsContiguous())
        {
            std::fill(data, end, value);
        }
        else
        {
            while (data < end)
            {
                *data = value;
                data += this->GetIncrement();
            }
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename GeneratorType>
    void VectorReference<ElementType, orientation>::Generate(GeneratorType generator)
    {
        ElementType* data = GetDataPointer();
        ElementType* end = data + this->Size() * this->GetIncrement();

        while (data < end)
        {
            *data = static_cast<ElementType>(generator());
            data += this->GetIncrement();
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename TransformationType>
    void VectorReference<ElementType, orientation>::Transform(TransformationType transformation)
    {
        ElementType* pData = this->GetDataPointer();
        ElementType* pEnd = pData + this->Size() * this->GetIncrement();
        if (this->IsContiguous())
        {
            std::transform(pData, pEnd, pData, transformation);
        }
        else
        {
            while (pData < pEnd)
            {
                *pData = transformation(*pData);
                pData += this->GetIncrement();
            }
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    VectorReference<ElementType, orientation> VectorReference<ElementType, orientation>::GetReference()
    {
        return VectorReference<ElementType, orientation>(GetDataPointer(), this->Size(), this->GetIncrement());
    }

    template <typename ElementType, VectorOrientation orientation>
    VectorReference<ElementType, orientation> VectorReference<ElementType, orientation>::GetSubVector(size_t offset, size_t size)
    {
        DEBUG_THROW(offset + size > this->Size(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return VectorReference<ElementType, orientation>(GetDataPointer() + offset * this->GetIncrement(), size, this->GetIncrement());
    }

    //
    // Vector
    //

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(size_t size) :
        VectorReference<ElementType, orientation>(nullptr, size, 1),
        _data(size)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::vector<ElementType> data) :
        VectorReference<ElementType, orientation>(nullptr, data.size(), 1),
        _data(std::move(data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::initializer_list<ElementType> list) :
        VectorReference<ElementType, orientation>(nullptr, list.size(), 1),
        _data(list.begin(), list.end())
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(Vector<ElementType, orientation>&& other) :
        VectorReference<ElementType, orientation>(nullptr, other.Size(), other.GetIncrement()),
        _data(std::move(other._data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const Vector<ElementType, orientation>& other) :
        VectorReference<ElementType, orientation>(nullptr, other.Size(), 1),
        _data(other.Size())
    {
        _pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const ConstVectorReference<ElementType, orientation>& other) :
        VectorReference<ElementType, orientation>(nullptr, other.Size(), 1),
        _data(other.Size())
    {
        _pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>& other) :
        VectorReference<ElementType, orientation>(nullptr, other.Size(), 1),
        _data(other.Size())
    {
        _pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    void Vector<ElementType, orientation>::Resize(size_t size)
    {
        _data.resize(size);
        this->_pData = _data.data();
        this->_size = size;
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>& Vector<ElementType, orientation>::operator=(Vector<ElementType, orientation> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, VectorOrientation orientation>
    void Vector<ElementType, orientation>::Swap(Vector<ElementType, orientation>& other)
    {
        VectorReference<ElementType, orientation>::Swap(other);
        std::swap(_data, other._data);
    }

    template <typename ElementType, VectorOrientation orientation>
    utilities::StlStridedIterator<typename std::vector<ElementType>::iterator> begin(Vector<ElementType, orientation>& vector)
    {
        return { vector._data.begin(), static_cast<ptrdiff_t>(vector.GetIncrement()) };
    }

    template <typename ElementType, VectorOrientation orientation>
    utilities::StlStridedIterator<typename std::vector<ElementType>::const_iterator> begin(const Vector<ElementType, orientation>& vector)
    {
        return { vector._data.cbegin(), static_cast<ptrdiff_t>(vector.GetIncrement()) };
    }

    template <typename ElementType, VectorOrientation orientation>
    utilities::StlStridedIterator<typename std::vector<ElementType>::iterator> end(Vector<ElementType, orientation>& vector)
    {
        return { vector._data.end(), static_cast<ptrdiff_t>(vector.GetIncrement()) };
    }

    template <typename ElementType, VectorOrientation orientation>
    utilities::StlStridedIterator<typename std::vector<ElementType>::const_iterator> end(const Vector<ElementType, orientation>& vector)
    {
        return { vector._data.cend(), static_cast<ptrdiff_t>(vector.GetIncrement()) };
    }

    //
    // VectorArchiver
    //
    template <typename ElementType, VectorOrientation orientation>
    void VectorArchiver::Write(const Vector<ElementType, orientation>& vector, const std::string& name, utilities::Archiver& archiver)
    {
        archiver[name] << vector.ToArray();
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorArchiver::Read(Vector<ElementType, orientation>& vector, const std::string& name, utilities::Unarchiver& archiver)
    {
        std::vector<ElementType> values;

        archiver[name] >> values;

        Vector<ElementType, orientation> value(std::move(values));

        vector.Swap(value);
    }
} // namespace math
} // namespace ell

#pragma endregion implementation
