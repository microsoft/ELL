////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TransformIterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // TransformIterator definitions
    //
    template <typename InputIteratorType, typename OutType, typename FuncType>
    TransformIterator<InputIteratorType, OutType, FuncType>::TransformIterator(InputIteratorType& inIter, FuncType transformFunction) : _inIter(inIter), _transformFunction(transformFunction)
    {}

    template <typename InputIteratorType, typename FnType>
    auto MakeTransformIterator(InputIteratorType& inIterator, FnType transformFunction) -> TransformIterator<InputIteratorType, decltype(transformFunction(inIterator.Get())), FnType>
    {
        using OutType = decltype(transformFunction(inIterator.Get()));
        return TransformIterator<InputIteratorType, OutType, FnType>(inIterator, transformFunction);
    }
}
