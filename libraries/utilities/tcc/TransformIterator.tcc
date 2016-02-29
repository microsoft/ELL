////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TransformIterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // TransformIterator definitions
    //
    template <typename InputIteratorType, typename OutType, typename Func>
    TransformIterator<InputIteratorType, OutType, Func>::TransformIterator(InputIteratorType& inIter, Func transformFn) : _inIter(inIter), _transformFn(transformFn)
    {}
    
    template <typename InputIteratorType, typename OutType, typename Func>
    bool TransformIterator<InputIteratorType, OutType, Func>::IsValid() const
    {
        return _inIter.IsValid();
    }

    template <typename InputIteratorType, typename OutType, typename Func>
    void TransformIterator<InputIteratorType, OutType, Func>::Next() 
    {
        _inIter.Next(); 
    };
    
    template <typename InputIteratorType, typename OutType, typename Func>
    OutType TransformIterator<InputIteratorType, OutType, Func>::Get() const
    {
        return _transformFn(_inIter.Get()); 
    }

    template <typename InputIteratorType, typename FnType>
    auto MakeTransform(InputIteratorType& inIterator, FnType transformFn) -> TransformIterator<InputIteratorType, decltype(transformFn(inIterator.Get())), FnType>
    {
        using OutType = decltype(transformFn(inIterator.Get()));
        return TransformIterator<InputIteratorType, OutType, FnType>(inIterator, transformFn);
    }
}
