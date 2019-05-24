////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOutputDataCache.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelOutputDataCache.h"

#include <model/include/InputPort.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Logger.h>

namespace ell
{
using namespace ell::model;

ModelOutputDataCache::ModelOutputDataCache() :
    _maxCacheSize(0)
{
}

ModelOutputDataCache::ModelOutputDataCache(int maxCacheSize) :
    _maxCacheSize(maxCacheSize)
{
}

bool ModelOutputDataCache::HasCachedData(const ell::model::OutputPortBase* port) const
{
    return _cache.find(port) != _cache.end();
}

const UnlabeledDataContainer& ModelOutputDataCache::GetCachedData(const ell::model::OutputPortBase* port) const
{
    ++_currentGeneration;
    _cache.at(port).generation = _currentGeneration;
    return _cache.at(port).data;
}

void ModelOutputDataCache::RemoveCachedData(const ell::model::OutputPortBase* port)
{
    _cache.erase(port);
}

void ModelOutputDataCache::SetCachedData(const ell::model::OutputPortBase* port, UnlabeledDataContainer data)
{
    // if the cache is too big, first remove entries
    while (_maxCacheSize > 0 && static_cast<int>(_cache.size()) >= _maxCacheSize)
    {
        RemoveLeastRecentlyUsedEntry();
    }

    _cache[port] = { _currentGeneration, data };
}

void ModelOutputDataCache::RemoveLeastRecentlyUsedEntry()
{
    using namespace logging;
    Log() << "Removing least-recently-used entry" << EOL;
    if (_cache.empty())
    {
        return;
    }

    auto lruEntry = _cache.begin();
    for (auto it = _cache.begin(); it != _cache.end(); ++it)
    {
        if (it->second.generation < lruEntry->second.generation)
        {
            lruEntry = it;
        }
    }
    _cache.erase(lruEntry);
}

const OutputPortBase* ModelOutputDataCache::FindNearestCachedOutputPort(const OutputPortBase* output)
{
    const OutputPortBase* currentOutput = output;
    while (true)
    {
        auto node = currentOutput->GetNode();
        if (node->NumInputPorts() > 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Nodes with >1 input not cacheable");
        }

        if (HasCachedData(currentOutput))
        {
            return currentOutput;
        }

        if (node->NumInputPorts() == 0)
        {
            return nullptr;
        }

        // TODO: deal with merge points -- need to skip parallel area and return something before path split.

        // For instance, in this graph:
        //     |
        //    (A)
        //     |
        //    (B)
        //   /   \
        // (C)   (D)
        //  |     |
        //  |    (E)
        //   \   /
        //    (F)
        //     |
        //    (G)
        //
        // the nearest cacheable output to (G) would be (B). So we need to find the nearest common ancestor to (C) and (E): the inputs of (F)

        // Solution: when you get to a split point, skip to nearest common ancestor

        currentOutput = &(node->GetInputPort(0)->GetReferencedPort());
    }
}
} // namespace ell
