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
    template <typename InType, typename OutType, typename Func>
    TransformIterator<InType, OutType, Func>::TransformIterator(IIterator<InType>& inIter, Func transformFn) : _inIter(inIter), _transformFn(transformFn)
    {}
    
    template <typename InType, typename OutType, typename Func>
    bool TransformIterator<InType, OutType, Func>::IsValid() const
    {
        return _inIter.IsValid();
    }

    template <typename InType, typename OutType, typename Func>
    void TransformIterator<InType, OutType, Func>::Next() 
    {
        _inIter.Next(); 
    };
    
    template <typename InType, typename OutType, typename Func>
    OutType TransformIterator<InType, OutType, Func>::Get() const
    {
        return _transformFn(_inIter.Get()); 
    }

    template <typename InType, typename FnType>
    auto MakeTransform(IIterator<InType>& inIterator, FnType transformFn) -> TransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>
    {
        return TransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>(inIterator, transformFn);
    }
}
