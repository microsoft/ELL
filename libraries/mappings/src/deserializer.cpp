// deserializer.cpp

#include "mappings.h"

#include <memory>
using std::shared_ptr;
using std::make_shared;

namespace mappings
{
    void Deserialize(JsonSerializer& serializer, std::shared_ptr<Map>& up)
    {
        up->Deserialize(serializer);
    }

    void DefaultDeserialize(JsonSerializer& serializer, shared_ptr<Layer>& up)
    {
        auto type = serializer.Read<string>("_type");
        auto version = serializer.Read<int>("_version");

        if (type == "Zero")
        {
            up = make_shared<Zero>();
        }

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

        else
        {
            throw runtime_error("unidentified type in map file: " + type);
        }

        up->Deserialize(serializer, version);
    }
}
