////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    //
    // UnorientedConstVectorBase
    //

    template <typename ElementType>
    UnorientedConstVectorBase<ElementType>::UnorientedConstVectorBase(const ElementType* pData, size_t size, size_t increment)
        : _pData(pData), _size(size), _increment(increment)
    {
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorBase<ElementType>::operator[](size_t index) const
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
    void ConstVectorReference<ElementType, orientation>::Swap(ConstVectorReference<ElementType, orientation>& other)
    {
        UnorientedConstVectorBase<ElementType>::Swap(other);
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
    void VectorReference<ElementType, orientation>::CopyFrom(ConstVectorReference<ElementType, orientation> other)
    {
        if (this->Size() != other.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const ElementType* pOtherData = other.GetConstDataPointer();
        const size_t otherIncrement = other.GetIncrement();
        const ElementType* pOtherEnd = pOtherData + otherIncrement * other.Size();

        if (this->GetIncrement() == 1 && otherIncrement == 1)
        {
            while (pOtherData < pOtherEnd)
            {
                (*pData) = (*pOtherData);
                ++pData;
                ++pOtherData;
            }
        }
        else
        {
            while (pOtherData < pOtherEnd)
            {
                (*pData) = (*pOtherData);
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
        const ElementType* pEnd = pData + this->Size() * this->GetIncrement();
        while (pData < pEnd)
        {
            *pData = transformation(*pData);
            pData += this->GetIncrement();
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
    Vector<ElementType, orientation>::Vector(size_t size)
        : VectorReference<ElementType, orientation>(nullptr, size, 1), _data(size)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::vector<ElementType> data)
        : VectorReference<ElementType, orientation>(nullptr, data.size(), 1), _data(std::move(data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::initializer_list<ElementType> list)
        : VectorReference<ElementType, orientation>(nullptr, list.size(), 1), _data(list.begin(), list.end())
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(Vector<ElementType, orientation>&& other)
        : VectorReference<ElementType, orientation>(nullptr, other.Size(), other.GetIncrement()), _data(std::move(other._data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const Vector<ElementType, orientation>& other)
        : VectorReference<ElementType, orientation>(nullptr, other.Size(), 1), _data(other.Size())
    {
        _pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(ConstVectorReference<ElementType, orientation>& other)
        : VectorReference<ElementType, orientation>(nullptr, other.Size(), 1), _data(other.Size())
    {
        _pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(ConstVectorReference<ElementType, TransposeVectorOrientation<orientation>::value>& other)
        : VectorReference<ElementType, orientation>(nullptr, other.Size(), 1), _data(other.Size())
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
}
}
