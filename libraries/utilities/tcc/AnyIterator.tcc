////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AnyIterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIterator.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

namespace utilities
{
    //
    // wrapper to convert anything that happens to conform to IIterator interface to an IIterator
    //
    template <typename IteratorType, typename ValueType>
    class IteratorWrapper : public IIterator<ValueType>
    {
    public:
        IteratorWrapper(const IteratorWrapper<IteratorType, ValueType>& other) = default;
        IteratorWrapper(IteratorWrapper<IteratorType, ValueType>&& other) = default;

        IteratorWrapper(IteratorType&& inputIterator)
            : _iterator(inputIterator) {}

        virtual bool IsValid() const override { return _iterator.IsValid(); }
        virtual bool HasSize() const override { return _iterator.HasSize(); }
        virtual uint64_t NumIteratesLeft() const override { return _iterator.NumIteratesLeft(); }
        virtual void Next() override { _iterator.Next(); }
        virtual ValueType Get() const override { return _iterator.Get(); }
    private:
        IteratorType _iterator;
    };

    //
    // AnyIterator class implementation
    //
    template <typename ValueType>
    AnyIterator<ValueType>::AnyIterator(std::shared_ptr<IIterator<ValueType>> iterator)
        : _iterator(iterator)
    {
    }

    template <typename ValueType>
    bool AnyIterator<ValueType>::IsValid() const
    {
        if (_iterator == nullptr)
        {
            std::string funcName = __func__;
            throw std::runtime_error(funcName + ": invalid iterator");
        }
        return _iterator->IsValid();
    }

    template <typename ValueType>
    bool AnyIterator<ValueType>::HasSize() const
    {
        if (_iterator == nullptr)
        {
            std::string funcName = __func__;
            throw std::runtime_error(funcName + ": invalid iterator");
        }
        return _iterator->HasSize();
    }

    template <typename ValueType>
    uint64_t AnyIterator<ValueType>::NumIteratesLeft() const
    {
        if (_iterator == nullptr)
        {
            std::string funcName = __func__;
            throw std::runtime_error(funcName + ": invalid iterator");
        }
        return _iterator->NumIteratesLeft();
    }

    template <typename ValueType>
    void AnyIterator<ValueType>::Next()
    {
        if (_iterator == nullptr)
        {
            std::string funcName = __func__;
            throw std::runtime_error(funcName + ": invalid iterator");
        }
        _iterator->Next();
    }

    template <typename ValueType>
    ValueType AnyIterator<ValueType>::Get() const
    {
        if (_iterator == nullptr)
        {
            std::string funcName = __func__;
            throw std::runtime_error(funcName + ": invalid iterator");
        }
        return _iterator->Get();
    }

    template <typename IteratorType, typename ValueType>
    AnyIterator<ValueType> MakeAnyIterator(IteratorType&& iter)
    {
        auto wrapper = std::make_shared<IteratorWrapper<IteratorType, ValueType>>(std::forward<IteratorType>(iter));
        return AnyIterator<ValueType>(wrapper);
    }
}
