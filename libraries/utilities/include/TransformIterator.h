////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace utilities
{
    /// <summary> A read-only forward iterator that transforms the items from an input collection </summary>
    template <typename InputIteratorType, typename OutType, typename FuncType>
    class TransformIterator
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="inIter"> An iterator for the input collection </param>
        /// <param name="transformFunction"> The function to apply to transform the input items</param>
        TransformIterator(InputIteratorType& inIter, FuncType transformFunction);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _inIter.IsValid(); }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next() { _inIter.Next(); }

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The result of applying the transformFunction on the current item in the input iterator. </returns>
        OutType Get() const { return _transformFunction(_inIter.Get()); }

    private:
        InputIteratorType& _inIter;
        FuncType _transformFunction;
    };

    /// <summary> Convenience function for creating TransformIterators </summary>
    ///
    /// <param name="inIter"> An iterator for the input collection </param>
    /// <param name="transformFunction"> The function to apply to transform the input items</param>
    ///
    /// <returns> A TransformIterator over the input sequence using the specified transform function</returns>
    template <typename InputIteratorType, typename FnType>
    auto MakeTransformIterator(InputIteratorType& inIterator, FnType transformFunction) -> TransformIterator<InputIteratorType, decltype(transformFunction(inIterator.Get())), FnType>;
}
}

#include "../tcc/TransformIterator.tcc"
