////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SymbolTable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType, ValueType DefaultValue>
    SymbolTable<ValueType, DefaultValue>::SymbolTable(std::initializer_list<SymbolValue> values)
        : _map(values)
    {
    }

    template <typename ValueType, ValueType DefaultValue>
    ValueType SymbolTable<ValueType, DefaultValue>::Get(const std::string& name) const
    {
        ValueType value = DefaultValue;
        auto search = _map.find(name);
        if (search != _map.end())
        {
            value = search->second;
        }
        return value;
    }

    template <typename ValueType, ValueType DefaultValue>
    void SymbolTable<ValueType, DefaultValue>::Add(const std::string& name, ValueType value)
    {
        if (_map.count(name) > 0)
        {
            throw EmitterException(EmitterError::duplicateSymbol);
        }
        _map[std::move(name)] = std::move(value);
    }

    template <typename ValueType, ValueType DefaultValue>
    std::string SymbolTable<ValueType, DefaultValue>::GetUniqueName(const std::string& namePrefix) const
    {
        // return namePrefix;
        int index = 0;
        while(true)
        {
            auto name = namePrefix + "_" + std::to_string(index);
            if(!Contains(name))
            {
                return name;
            }
            ++index;
        }
    }

    template <typename ValueType, ValueType DefaultValue>
    bool SymbolTable<ValueType, DefaultValue>::Contains(const std::string& name) const
    {
        return (Get(name) != DefaultValue);
    }


    template <typename ValueType, ValueType DefaultValue>
    void SymbolTable<ValueType, DefaultValue>::Remove(const std::string& name)
    {
        auto search = _map.find(name);
        if (search != _map.end())
        {
            _map.erase(search);
        }
    }

    template <typename ValueType, ValueType DefaultValue>
    void SymbolTable<ValueType, DefaultValue>::Clear()
    {
        _map.clear();
    }
}
}
