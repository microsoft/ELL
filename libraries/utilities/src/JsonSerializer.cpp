// jason_serializer.cpp

#include "JsonSerializer.h"

#include <stdexcept>
using std::runtime_error;

namespace utilities
{

    std::string JsonSerializer::ToString() const
    {
        try
        {
            return _json_value.toStyledString();
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during serialization");
        }
    }

    void JsonSerializer::FromString(const std::string& s)
    {
        try
        {
            Json::Reader json_reader;
            json_reader.parse(s, _json_value);
        }
        catch (...)    // underlying json implementation may throw an exception 
        {
            throw runtime_error("jsoncpp threw an unspecified exception during deserialization");
        }
    }
}