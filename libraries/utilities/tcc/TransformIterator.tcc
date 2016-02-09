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
    OutType TransformIterator<InType, OutType, Func>::Get() 
    {
        return _transformFn(_inIter.Get()); 
    }
}
