////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Optional.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace utilities
{
    template <typename T>
    const T& Optional<T>::GetValue() const 
    { 
        if (!HasValue())
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Error: called GetValue on an optional object without a value");
        }
        return _value; 
    }
    
    template <typename T>
    const T& Optional<T>::GetValue(const T& defaultValue) const 
    { 
        if (!HasValue())
        {
            return defaultValue;
        }
        return _value; 
    }
    
    template <typename T>
    void Optional<T>::SetValue(const T& value)
    {
        _value = value; 
        _hasValue = true;
    }
    
    template <typename T>
    void Optional<T>::Clear()
    {
        _value = T(); 
        _hasValue = false; 
    }
}
}
