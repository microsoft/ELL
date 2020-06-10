////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndexRange.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/IndexRange.h"

namespace ell
{
namespace value
{
    namespace loopnests
    {
        IndexRange::IndexRange(const Index& index, const Range& range) :
            _index(index),
            _range(range)
        {
        }

        IndexRange::IndexRange(const std::string& name, const Range& range) :
            _index({ name }),
            _range(range)
        {
        }

        const Index& IndexRange::GetIndex() const
        {
            return _index;
        }

        const std::string& IndexRange::GetName() const
        {
            return _index.GetName();
        }

        int IndexRange::Begin() const
        {
            return _range.Begin();
        }

        int IndexRange::End() const
        {
            return _range.End();
        }

        int IndexRange::Size() const
        {
            return _range.Size();
        }

        int IndexRange::Increment() const
        {
            return _range.Increment();
        }

        Range IndexRange::GetRange() const
        {
            return _range;
        }
    } // namespace loopnests
} // namespace value
} // namespace ell
