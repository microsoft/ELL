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
    // UnorientedConstVectorReference
    //

    template <typename ElementType>
    UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference(const ElementType* pData, size_t size, size_t increment)
        : _pData(pData), _size(size), _increment(increment)
    {
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::operator[](size_t index) const
    {
        DEBUG_THROW(index >= _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return GetConstDataPointer()[index * _increment];
    }

    template <typename ElementType>
    void UnorientedConstVectorReference<ElementType>::Swap(UnorientedConstVectorReference<ElementType>& other)
    {
        std::swap(_pData, other._pData);
        std::swap(_size, other._size);
        std::swap(_increment, other._increment);
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm0() const
    {
        return Aggregate([](ElementType x) { return x != 0 ? 1 : 0; });
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm1() const
    {
        return Aggregate([](ElementType x) { return std::abs(x); });
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm2() const
    {
        return std::sqrt(Norm2Squared());
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm2Squared() const
    {
        return Aggregate([](ElementType x) { return x*x; });
    }

    template <typename ElementType>
    template <typename MapperType>
    ElementType UnorientedConstVectorReference<ElementType>::Aggregate(MapperType mapper) const
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
    std::vector<ElementType> UnorientedConstVectorReference<ElementType>::ToArray() const
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
        UnorientedConstVectorReference<ElementType>::Swap(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::IsEqual(ConstVectorReference<ElementType, orientation> other, ElementType tolerance) const
    {
        if (_size != other._size)
        {
            return false;
        }

        const ElementType* pThis = GetConstDataPointer();
        const ElementType* pThisEnd = pThis + _size * _increment;
        const ElementType* pOther = other.GetConstDataPointer();

        while (pThis < pThisEnd)
        {
            auto diff = (*pThis) - (*pOther);

            if (diff > tolerance || -diff > tolerance)
            {
                return false;
            }
            pThis += _increment;
            pOther += other._increment;
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
        DEBUG_THROW(offset + size > _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return ConstVectorReference<ElementType, orientation>(GetConstDataPointer() + offset * _increment, size, _increment);
    }
    //
    // TransformedConstVectorReference
    // 

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType>::TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation)
        : _vector(vector), _transformation(std::move(transformation))
    {
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation)
    {
        return TransformedConstVectorReference<ElementType, orientation, TransformationType>(vector, transformation);
    }

    //
    // VectorReference
    // 

    template <typename ElementType, VectorOrientation orientation>
    ElementType& VectorReference<ElementType, orientation>::operator[](size_t index)
    {
        DEBUG_THROW(index >= _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return GetDataPointer()[index * _increment];
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::Swap(VectorReference<ElementType, orientation>& other)
    {
        ConstVectorReference<ElementType, orientation>::Swap(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename TransformationType>
    void VectorReference<ElementType, orientation>::CopyFrom(TransformedConstVectorReference<ElementType, orientation, TransformationType> other)
    {
        const auto& otherVector = other.GetVector();
        auto transformation = other.GetTransformation();

        DEBUG_THROW(_size != otherVector.Size(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size."));

        ElementType* pData = GetDataPointer();
        const ElementType* pEnd = pData + _increment * _size;
        const ElementType* pOtherData = otherVector.GetConstDataPointer();
        const size_t otherIncrement = otherVector.GetIncrement();

        while (pData < pEnd)
        {
            (*pData) = transformation(*pOtherData);
            pData += _increment;
            pOtherData += otherIncrement;
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::CopyFrom(ConstVectorReference<ElementType, orientation> other)
    {
        if (_size != other.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const ElementType* pOtherData = other.GetConstDataPointer();
        const size_t otherIncrement = other.GetIncrement();
        const ElementType* pOtherEnd = pOtherData + otherIncrement * other.Size();

        if (_increment == 1 && otherIncrement == 1)
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
                pData += _increment;
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
        ElementType* end = data + _size * _increment;

        if (_increment == 1)
        {
            std::fill(data, end, value);
        }
        else
        {
            while (data < end)
            {
                *data = value;
                data += _increment;
            }
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename GeneratorType>
    void VectorReference<ElementType, orientation>::Generate(GeneratorType generator)
    {
        ElementType* data = GetDataPointer();
        ElementType* end = data + _size * _increment;

        while (data < end)
        {
            *data = static_cast<ElementType>(generator());
            data += _increment;
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename TransformationType>
    void VectorReference<ElementType, orientation>::Transform(TransformationType transformation)
    {
        ElementType* current = GetDataPointer();
        const ElementType* end = current + _size * _increment;
        while (current < end)
        {
            *current = transformation(*current);
            current += _increment;
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    VectorReference<ElementType, orientation> VectorReference<ElementType, orientation>::GetReference()
    {
        return VectorReference<ElementType, orientation>(GetDataPointer(), _size, _increment);
    }

    template <typename ElementType, VectorOrientation orientation>
    VectorReference<ElementType, orientation> VectorReference<ElementType, orientation>::GetSubVector(size_t offset, size_t size)
    {
        DEBUG_THROW(offset + size > _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return VectorReference<ElementType, orientation>(GetDataPointer() + offset * _increment, size, _increment);
    }

    template <typename ElementType, VectorOrientation orientation>
    template <typename TransformationType>
    void VectorReference<ElementType, orientation>::operator+=(TransformedConstVectorReference<ElementType, orientation, TransformationType> other)
    {
        const auto& otherVector = other.GetVector();
        auto transformation = other.GetTransformation();

        if (_size != otherVector.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const ElementType* pEnd = pData + _increment * _size;
        const ElementType* pOtherData = otherVector.GetConstDataPointer();
        const size_t otherIncrement = otherVector.GetIncrement();

        while (pData < pEnd)
        {
            (*pData) += transformation(*pOtherData);
            pData += _increment;
            pOtherData += otherIncrement;
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator+=(ConstVectorReference<ElementType, orientation> other)
    {
        if (_size != other.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const ElementType* pEnd = pData + _increment * _size;
        const ElementType* pOtherData = other.GetConstDataPointer();
        const size_t otherIncrement = other.GetIncrement();

        while (pData < pEnd)
        {
            (*pData) += (*pOtherData);
            pData += _increment;
            pOtherData += otherIncrement;
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator-=(ConstVectorReference<ElementType, orientation> other)
    {
        if (_size != other.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "this vector and other vector are not the same size.");
        }

        ElementType* pData = GetDataPointer();
        const ElementType* pEnd = pData + _increment * _size;
        const ElementType* pOtherData = other.GetConstDataPointer();
        const size_t otherIncrement = other.GetIncrement();

        while (pData < pEnd)
        {
            (*pData) -= (*pOtherData);
            pData += _increment;
            pOtherData += otherIncrement;
        }
    }

    //
    // Vector
    //

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(size_t size)
        : VectorReference<ElementType, orientation>(nullptr, size, 1), _data(size)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::vector<ElementType> data)
        : VectorReference<ElementType, orientation>(nullptr, data.size(), 1), _data(std::move(data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::initializer_list<ElementType> list)
        : VectorReference<ElementType, orientation>(nullptr, list.size(), 1), _data(list.begin(), list.end())
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(Vector<ElementType, orientation>&& other)
        : VectorReference<ElementType, orientation>(nullptr, other._size, other._increment), _data(std::move(other._data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const Vector<ElementType, orientation>& other)
        : VectorReference<ElementType, orientation>(nullptr, other._size, other._increment), _data(other._data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    void Vector<ElementType, orientation>::Resize(size_t size)
    {
        _data.resize(size);
        _pData = _data.data();
        _size = size;
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