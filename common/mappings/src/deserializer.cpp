// deserializer.cpp

#include "mappings.h"
#include <stdexcept>

using std::shared_ptr;
using std::make_shared;


// this code deliberately commented out, and should appear in any Project that uses mappings with default serialization
// ====================================================================================================================
// void mappings::Deserialize(JsonSerializer& js, std::shared_ptr<Mapping>& up)
// {
//     mappings::DefaultDeserialize(js, up);
// }

namespace mappings
{
    void DefaultDeserialize(JsonSerializer& js, shared_ptr<Mapping>& up)
    {
        auto type = js.read<string>("_type");
        auto version = js.read<int>("_version");

        if (type == "Constant")
        {
            up = make_shared<Constant>();
        }

        else if(type == "Scale")
        {
            up = make_shared<Scale>();
        }

        else if(type == "Shift")
        {
            up = make_shared<Shift>();
        }

        else if(type == "Sum")
        {
            up = make_shared<Sum>();
        }

        else if (type == "DecisionTreePath")
        {
            up = make_shared<DecisionTreePath>();
        }

        else if (type == "Column")
        {
            up = make_shared<Column>();
        }

        else if (type == "Row")
        {
            up = make_shared<Row>();
        }

        else
        {
            throw runtime_error("unidentified type in map file: " + type);
        }

        up->Deserialize(js, version);
    }
}
