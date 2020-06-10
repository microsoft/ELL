////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CachingProvider.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CachingProvider.h"
#include "LoopNests.h"

#include "loopnests/LoopNest.h"

namespace ell
{
namespace value
{
    void CachingProvider::Initialize(ViewAdapter view, utilities::MemoryShape cacheShape, utilities::DimensionOrder order, std::vector<Index> kernelIndices, std::vector<Index> atIndices, std::any extra)
    {
        _value = view;
        _shape = cacheShape;
        _order = order;
        _kernelIndices = kernelIndices;
        _atIndices = atIndices;
        _extra = extra;
    }

    void CachingProvider::HandleCaching(LoopNest& loopnest)
    {
        for (auto& index : _kernelIndices)
        {
            index = loopnest.GetUnderlyingLoopNest().GetBaseIndex(index);
        }

        HandleCachingImpl(loopnest);
    }

} // namespace value
} // namespace ell