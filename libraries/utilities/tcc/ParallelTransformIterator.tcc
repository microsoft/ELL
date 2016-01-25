// ParallelTransformIterator.tcc

using std::async;

#include <iostream>
using std::cerr;
using std::endl;

namespace utilities
{
    //
    // ParallelTransformIterator definitions
    //


    // TODO: instead of just having buffer of futures, have buffer of
    // output values. Otherwise, we can't instantiate a new future
    // until we've retrieved the value of the last one.

    // have the async call apply the transform fn and store it in the
    // correct place in the buffer. The buffer could have more
    // entries than there are threads and still be useful.

    // Or does the current solution really work fine? 

    template <typename InType, typename OutType, typename Func>
    ParallelTransformIterator<InType, OutType, Func>::ParallelTransformIterator(IIterator<InType>& inIter, Func transformFn) : _inIter(inIter), _transformFn(transformFn), _currentIndex(0), _endIndex(-1), _currentOutputValid(false)
    {
        // Fill the buffer with futures that are the result of calling async(transformFn) on inIter
        int index = 0;
        for(int index = 0; index < _maxSize; index++)
        {
            if(!_inIter.IsValid())
            {
                _endIndex = index;
                break;
            }

            _futures.emplace_back(async(_transformFn, _inIter.Get()));
            _inIter.Next();
        }
    }
    
    template <typename InType, typename OutType, typename Func>
    bool ParallelTransformIterator<InType, OutType, Func>::IsValid() const
    {
        return _currentIndex != _endIndex;
    }

    template <typename InType, typename OutType, typename Func>
    void ParallelTransformIterator<InType, OutType, Func>::Next() 
    {
        if(!IsValid())
        {
            return;
        }
        _currentOutputValid = false;
        
        // If necessary, create new future to handle next input
        if(_inIter.IsValid())
        {
            _futures[_currentIndex] = async(_transformFn, _inIter.Get());
            _inIter.Next();
        }
        else
        {
            if(_endIndex < 0) // yuck
            {
                _endIndex = _currentIndex;
            }
        }
        _currentIndex = (_currentIndex+1)%_maxSize;
    };
    
    template <typename InType, typename OutType, typename Func>
    OutType ParallelTransformIterator<InType, OutType, Func>::Get() 
    {
        if(!_currentOutputValid)
        {
            _currentOutput = _futures[_currentIndex].get();
            _currentOutputValid = true;
        }

        return _currentOutput;
    }
}
