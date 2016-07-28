////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace utilities
{
    //
    // Serializer class
    //

    //
    // SimpleSerializer
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleSerializer::SerializeFundamental(const char* name, const ValueType& value)
    {
        using std::to_string;
        if(name == std::string(""))
        {
            std::cout << to_string(value) << std::endl;
        }
        else
        {
            std::cout << name << ": " << to_string(value) << std::endl;
        }
    }
}
