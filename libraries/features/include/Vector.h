////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <iostream>

namespace features
{
    // idea: allow backing store to be either vector or array
    // crikey: is it at all possible to make a version that just keeps a reference to a std::vector that gets passed in. Only useful (?) if we know we'll only last as long or less than the input.

    template <typename T>
    class Vector
    {
    public:
        using size_type = typename std::vector<T>::size_type;
        using reference = typename std::vector<T>::reference;
        using const_reference = typename std::vector<T>::const_reference;
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

        // copy / move from std::vector
        Vector(const std::vector<T>& v) : _data(v) {};

        Vector(std::vector<T>&& v) : _data(std::forward<std::vector<T>>(v)) {};

        // op std::vector
        operator std::vector<T>() const & 
        { 
            return _data;
        } // copy

        operator std::vector<T>&& () &&
        {
            return std::move(_data);
        }; // move

        // Size
        size_type Size() { return _data.size(); }

        // op[]
        reference operator[](size_type n) { return _data[n]; }
        const_reference operator[](size_type n) const { return _data[n]; }

        // iterators
        iterator begin() { return _data.begin(); }
        const_iterator begin() const { return _data.begin(); }

        iterator end() { return _data.end(); }
        const_iterator end() const { return _data.end(); }
 
        // math ops
        // general vector operations
        double LengthSquared();
        double Length(); // L2 length

        // +=, -=, *=, /=
        // ==, !=
        // <, > ? lexical?
        

        // statistical operations
        // mean, var, stddev, median

    private:
        std::vector<T> _data;
    };
}

#include "../tcc/Vector.tcc"
