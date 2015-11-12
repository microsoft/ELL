// JsonSerializer.tcc

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::move;

namespace utilities
{
    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const ValueType& value, typename enable_if<is_class<ValueType>::value>::type* SFINAE)
    {
        try
        {
            JsonSerializer sub_serializer;
            value.Serialize(sub_serializer);
            _json_value[key] = sub_serializer._json_value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Write(KeyType key, const string& value)
    {
        try
        {
            _json_value[key] = value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const shared_ptr<ValueType>& ptr, typename enable_if<is_class<ValueType>::value>::type* SFINAE)
    {
        Write(key, *ptr);
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const ValueType& value, typename enable_if<is_fundamental<ValueType>::value>::type* SFINAE)
    {
        try
        {
            _json_value[key] = value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const ValueType& value, typename enable_if<is_enum<ValueType>::value>::type* SFINAE)
    {
        try
        {
            _json_value[key] = (int)value;
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Write(KeyType key, const vector<ValueType>& vec)
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
            throw runtime_error("jsoncpp threw an unspecified exception during write");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, ValueType& value, typename enable_if<is_class<ValueType>::value>::type* SFINAE) const
    {
        try
        {    
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];
            value.Deserialize(sub_serializer);
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Read(KeyType key, string& value) const
    {
        Get(key, value);
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, shared_ptr<ValueType>& ptr, typename enable_if<is_class<ValueType>::value>::type* SFINAE) const
    {
        try
        {
            JsonSerializer sub_serializer;
            sub_serializer._json_value = _json_value[key];
            Deserialize(sub_serializer, ptr);
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, ValueType& value, typename enable_if<is_fundamental<ValueType>::value>::type* SFINAE) const
    {
        Get(key, value);
    }

    template<typename ValueType, typename KeyType>
    ValueType JsonSerializer::Read(KeyType key, typename enable_if<is_default_constructible<ValueType>::value>::type* SFINAE) const
    {
        ValueType val;
        Get(key, val);
        return val;
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, ValueType& value, typename enable_if<is_enum<ValueType>::value>::type* SFINAE) const
    {
        int ival;
        Get(key, ival);
        value = (ValueType)ival;
    }

    template<typename ValueType, typename KeyType>
    ValueType JsonSerializer::Read(KeyType key, typename enable_if<is_enum<ValueType>::value>::type* SFINAE) const
    {
        int val;
        Get(key, val);
        return (ValueType)val;
    }

    template<typename KeyType, typename ValueType>
    void JsonSerializer::Read(KeyType key, vector<ValueType>& vec) const
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
                vec.push_back(move(val));
            }
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during read");
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
            throw runtime_error("jsoncpp threw an unspecified exception during read");
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
            throw runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, unsigned int& value) const
    {
        try
        {
            value = _json_value[key].asUInt();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during read");
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
            throw runtime_error("jsoncpp threw an unspecified exception during read");
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
            throw runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }

    template<typename KeyType>
    void JsonSerializer::Get(KeyType key, string& value) const
    {
        try
        {
            value = _json_value[key].asString();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during read");
        }
    }
}