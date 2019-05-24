////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOutputDataCache.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <model/include/OutputPort.h>

#include <unordered_map>

namespace ell
{
/// <summary> Caches the results from running a dataset through a model. </summary>
class ModelOutputDataCache
{
public:
    ModelOutputDataCache();
    ModelOutputDataCache(int maxCacheSize);
    bool HasCachedData(const ell::model::OutputPortBase* port) const;
    const UnlabeledDataContainer& GetCachedData(const ell::model::OutputPortBase* port) const;
    void RemoveCachedData(const ell::model::OutputPortBase* port);
    void SetCachedData(const ell::model::OutputPortBase* port, UnlabeledDataContainer data);

    // ??
    const ell::model::OutputPortBase* FindNearestCachedOutputPort(const ell::model::OutputPortBase* output);

private:
    void RemoveLeastRecentlyUsedEntry();

    struct CacheEntry
    {
        int64_t generation;
        UnlabeledDataContainer data;
    };
    mutable std::unordered_map<const ell::model::OutputPortBase*, CacheEntry> _cache;
    mutable int64_t _currentGeneration = 0;
    int _maxCacheSize = 0;
};
} // namespace ell
