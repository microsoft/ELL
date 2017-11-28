////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PropertyBag.h"
#include "Exception.h"

namespace ell
{
namespace utilities
{
    //
    // KeyValue
    //
    PropertyBag::KeyValue::KeyValue(const std::string& key, const Variant& value) : key(key), value(value)
    {
    }

    void PropertyBag::KeyValue::WriteToArchive(Archiver& archiver) const
    {
        archiver["k"] << key;
        archiver["v"] << value;
    }

    void PropertyBag::KeyValue::ReadFromArchive(Unarchiver& archiver)
    {
        archiver["k"] >> key;
        archiver["v"] >> value;
    }

    //
    // PropertyBag
    //
    Variant& PropertyBag::operator[](const std::string& key)
    {
        return _metadata[key];
    }

    bool PropertyBag::IsEmpty() const
    {
        if(_metadata.empty())
        {
            return true;
        }
        for(const auto& keyValue: _metadata)
        {
            if(!keyValue.second.IsEmpty())
            {
                return false;
            }
        }
        return true;
    }

    Variant PropertyBag::RemoveEntry(const std::string& key)
    {
        Variant result;
        auto keyIter = _metadata.find(key);
        if(keyIter != _metadata.end())
        {
            result = keyIter->second;
            _metadata.erase(keyIter);
        }        
        return result;
    }

    bool PropertyBag::HasEntry(const std::string& key) const
    {
        auto keyIter = _metadata.find(key);
        return (keyIter != _metadata.end()) && (!keyIter->second.IsEmpty());
    }

    void PropertyBag::WriteToArchive(Archiver& archiver) const
    {
        std::vector<KeyValue> keyValuePairs;
        for(const auto& pair: _metadata)
        {
            keyValuePairs.emplace_back(pair.first, pair.second);
        }
        archiver["data"] << keyValuePairs;
    }

    void PropertyBag::ReadFromArchive(Unarchiver& archiver)
    {
        _metadata.clear();
        std::vector<KeyValue> keyValuePairs;
        archiver["data"] >> keyValuePairs;
        for(const auto& pair: keyValuePairs)
        {
            _metadata[pair.key] = pair.value;
        }
    }
}
}

