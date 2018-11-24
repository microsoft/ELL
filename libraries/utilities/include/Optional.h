////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Optional.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Exception.h"

namespace ell
{
namespace utilities
{
    /// <summary> An optional value. </summary>
    template <typename T>
    class Optional
    {
    public:
        /// <summary> Default constructor: creates an empty value </summary>
        constexpr Optional() {}

        /// <summary> Constructor with a (non-'auto') value </summary>
        ///
        /// <param name="value"> The value to set the object to. </param>
        constexpr Optional(T value) :
            _hasValue(true),
            _value(value) {}

        /// <summary> Query if the object has a value </summary>
        ///
        /// <returns> A boolean indicating if this object has a value set. </returns>
        bool HasValue() const { return _hasValue; }

        /// <summary> Get the value stored in the object. If it doesn't have a stored value, an exception is thrown. </summary>
        ///
        /// <returns> The stored value. </returns>
        const T& GetValue() const;

        /// <summary> Get the value stored in the object. If it doesn't have a stored value, return the supplied default value. </summary>
        ///
        /// <param name="defaultValue"> The value to return if this object is empty. </param>
        ///
        /// <returns> The stored value, if there is one, otherwise returns the default value supplied. </returns>
        const T& GetValue(const T& defaultValue) const;

        /// <summary> Sets the value stored in the object. </summary>
        ///
        /// <param name="value"> The value to set this object to. </param>
        void SetValue(const T& value);

        /// <summary> Clears the value stored in the object. </summary>
        void Clear();

    private:
        bool _hasValue = false;
        T _value = {};
    };
} // namespace utilities
} // namespace ell

#include "../tcc/Optional.tcc"
