////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Iterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename ValueType>
    bool Iterator<ValueType>::IsValid()
    {
        return _isValid();
    }

    template <typename ValueType>
    void Iterator<ValueType>::Next()
    {
        _next();
    }

    template <typename ValueType>
    ValueType Iterator<ValueType>::Get()
    {
        return _get();
    }

    template <typename ValueType>
    Iterator<ValueType>::Iterator(std::shared_ptr<void> mem, std::function<bool()> isValid, std::function<void()> next, std::function<ValueType()> get) : _isValid(isValid), _next(next), _get(get), _originalIterator(mem)
    {
    }

    template <typename IteratorType, typename ValueType>
    Iterator<ValueType> MakeIterator(IteratorType iter)
    {
        std::shared_ptr<IteratorType> iterPtr = std::make_shared<IteratorType>(iter);
        auto isValid = [iterPtr]() {return iterPtr->IsValid();};
        auto next = [iterPtr]() { iterPtr->Next();};
        auto get = [iterPtr]() { return iterPtr->Get();};

        return Iterator<ValueType>(iterPtr, isValid, next, get);
    }
}
