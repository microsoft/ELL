// deserializer.cpp

#include "mappings.h"

#include <memory>
using std::shared_ptr;
using std::make_shared;


// this code deliberately commented out, and should appear in any Project that uses mappings with default serialization
// ====================================================================================================================
// void mappings::Deserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up)
// {
//     mappings::DefaultDeserialize(serializer, up);
// }

namespace mappings
{
    void DefaultDeserialize(JsonSerializer& serializer, shared_ptr<Layer>& up)
    {
        auto type = serializer.Read<string>("_type");
        auto version = serializer.Read<int>("_version");

        //if (type == "Constant")
        //{
        //    up = make_shared<Constant>();
        //}

        if(type == "Scale")
        {
            up = make_shared<Scale>();
        }

        else if(type == "Shift")
        {
            up = make_shared<Shift>();
        }

        //else if(type == "Sum")
        //{
        //    up = make_shared<Sum>();
        //}

        //else if (type == "DecisionTreePath")
        //{
        //    up = make_shared<DecisionTreePath>();
        //}

        //else if (type == "Column")
        //{
        //    up = make_shared<Column>();
        //}

        //else if (type == "Row")
        //{
        //    up = make_shared<Row>();
        //}

        // TODO add null
        // TODO check capitalization of these type strings

        else
        {
            throw runtime_error("unidentified type in map file: " + type);
        }

        up->Deserialize(serializer, version);
    }
}
