////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ObjectArchiver.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    //
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::WriteScalar(const char* name, const ValueType& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // Specialization for bool (though perhaps this should be an overload, not a specialization)
    template <>
    inline void ObjectArchiver::WriteScalar(const char* name, const bool& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void ObjectArchiver::WriteScalar(const char* name, const char* value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << std::string{ value };
        }
        else
        {
            _objectDescription[name] << std::string{ value };
        }
    }

    inline void ObjectArchiver::WriteScalar(const char* name, const std::string& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    template <typename ValueType>
    void ObjectArchiver::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription << array;
        }
        else
        {
            _objectDescription[name] << array;
        }
    }

    //
    // Deserialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::ReadScalar(const char* name, ValueType& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> value;
        }
        else
        {
            _objectDescription[name] >> value;
        }
    }

    template <>
    inline void ObjectArchiver::ReadScalar(const char* name, bool& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> value;
        }
        else
        {
            _objectDescription[name] >> value;
        }
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void ObjectArchiver::ReadScalar(const char* name, std::string& value)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> value;
        }
        else
        {
            _objectDescription[name] >> value;
        }
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void ObjectArchiver::ReadArray(const char* name, std::vector<ValueType>& array)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> array;
        }
        else
        {
            _objectDescription[name] >> array;
        }
    }

    inline void ObjectArchiver::ReadArray(const char* name, std::vector<std::string>& array)
    {
        if (std::string{ "" } == name)
        {
            _objectDescription >> array;
        }
        else
        {
            _objectDescription[name] >> array;
        }
    }
}
}
