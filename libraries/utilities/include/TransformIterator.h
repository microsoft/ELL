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
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // TransformIterator definitions
    //
    template <typename InputIteratorType, typename OutType, typename FuncType>
    TransformIterator<InputIteratorType, OutType, FuncType>::TransformIterator(InputIteratorType& inIter, FuncType transformFunction) :
        _inIter(inIter),
        _transformFunction(transformFunction)
    {
    }

    template <typename InputIteratorType, typename FnType>
    auto MakeTransformIterator(InputIteratorType& inIterator, FnType transformFunction) -> TransformIterator<InputIteratorType, decltype(transformFunction(inIterator.Get())), FnType>
    {
        using OutType = decltype(transformFunction(inIterator.Get()));
        return TransformIterator<InputIteratorType, OutType, FnType>(inIterator, transformFunction);
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation
