// deserializer.cpp

#include "deserializer.h"
#include "print.h"
#include <memory>
#include <stdexcept>

using std::make_shared;
using std::runtime_error;

void layers::Deserialize(JsonSerializer& serializer, std::shared_ptr<layers::Layer>& up)
{
    auto type = serializer.Read<string>("_type");

    if (type == "Zero")
    {
        up = make_shared<PrintableZero>();
    }

    else if(type == "Scale")
    {
        up = make_shared<PrintableScale>();
    }

    else if(type == "Shift")
    {
        up = make_shared<PrintableShift>();
    }

    else if(type == "Sum")
    {
        up = make_shared<PrintableSum>();
    }

    auto version = serializer.Read<int>("_version");
    up->Deserialize(serializer, version);
}

