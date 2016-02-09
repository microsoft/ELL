////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     JsonSerializer.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <stdexcept>

namespace utilities
{
    template<typename Type>
    Type JsonSerializer::Load(std::istream& is, std::string name)
    {
        // parse stream contents
        auto str = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        JsonSerializer reader;
        reader.FromString(str);

        // read object
        Type t;
        reader.Read(name, t);
        return std::move(t);
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const std::shared_ptr<ValueType>& ptr, typename std::enable_if<std::is_class<ValueType>::value>::type* concept)
    {
        Write(key, *ptr);
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept)
    {
        try
        {
            JsonSerializer sub_serializer;
            value.Serialize(sub_serializer);
            _json_value[key] = sub_serializer._json_value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Write(KeyType key, const std::string& value)
    {
        try
        {
            _json_value[key] = value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept)
    {
        try
        {
            _json_value[key] = value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const std::vector<ValueType>& vec)
    {
        try
        {
            JsonSerializer sub_serializer;
            for (size_t i = 0; i < vec.size(); ++i)
            {
                sub_serializer.Write((int)i, vec[i]);
            }
            _json_value[key] = sub_serializer._json_value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept) const
    {
        try
        {    
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];
            value.Deserialize(sub_serializer);
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Read(KeyType key, std::string& value) const
    {
        Get(key, value);
    }

    template<typename KeyType, typename ValueType, typename DeserializerType>
    void JsonSerializer::Read(KeyType key, std::shared_ptr<ValueType>& ptr, DeserializerType deserializer) const
    {
        try
        {
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];
            deserializer(sub_serializer, ptr);
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept) const
    {
        Get(key, value);
    }

    template<typename ValueType, typename KeyType>
    ValueType JsonSerializer::Read(KeyType key, typename std::enable_if<std::is_default_constructible<ValueType>::value>::type* concept) const
    {
        ValueType val;
        Get(key, val);
        return val;
    }

    template<typename KeyType, typename ValueType, typename DeserializerType>
    void JsonSerializer::Read(KeyType key, std::vector<std::shared_ptr<ValueType>>& vec, DeserializerType deserializer) const
    {
        try
        {
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];

            vec.clear();
            vec.reserve(sub_serializer._json_value.size());
            for (size_t i = 0; i < sub_serializer._json_value.size(); ++i)
            {
                std::shared_ptr<ValueType> val = nullptr;
                sub_serializer.Read((int)i, val, deserializer);
                vec.push_back(std::move(val));
            }
        }
        catch (std::runtime_error e)
        {
            throw; // rethrow the exception
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, std::vector<ValueType>& vec) const
    {
        try
        {
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];

            vec.clear();
            vec.reserve(sub_serializer._json_value.size());
            for (size_t i = 0; i < sub_serializer._json_value.size(); ++i)
            {
                ValueType val;
                sub_serializer.Read((int)i, val);
                vec.push_back(std::move(val));
            }
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, bool& value) const
    {
        try
        {
            value = _json_value[key].asBool();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, int& value) const
    {
        try
        {
            value = _json_value[key].asInt();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, unsigned int& value) const
    {
        try
        {
            value = _json_value[key].asUInt();
        }
        catch(...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, uint64& value) const
    {
        try
        {
            value = _json_value[key].asUInt64();
        }
        catch(...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, float& value) const
    {
        try
        {
            value = _json_value[key].asFloat();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, double& value) const
    {
        try
        {
            value = _json_value[key].asDouble();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, std::string& value) const
    {
        try
        {
            value = _json_value[key].asString();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw std::runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }
}
