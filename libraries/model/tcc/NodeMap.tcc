////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NodeMap.tcc (model)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename T, T defaultValue>
    T NodeMap<T, defaultValue>::Get(const model::Node& node) const
    {
        T value = defaultValue;
        auto search = _map.find(node.GetId());
        if (search != _map.end())
        {
            value = search->second;
        }
        return value;
    }

    template <typename T, T defaultValue>
    void NodeMap<T, defaultValue>::Set(const model::Node& node, T value)
    {
        _map[node.GetId()] = value;
    }

    template <typename T, T defaultValue>
    bool NodeMap<T, defaultValue>::Contains(const model::Node& node) const
    {
        return (Get(node) != nullptr);
    }

    template <typename T, T defaultValue>
    void NodeMap<T, defaultValue>::Remove(const model::Node& node)
    {
        auto search = _map.find(node.GetId());
        if (search != _map.end())
        {
            _map.erase(search);
        }
    }

    template <typename T, T defaultValue>
    void NodeMap<T, defaultValue>::Clear()
    {
        _map.clear();
    }
}
}