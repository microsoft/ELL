// deserializer.cpp

#include "deserializer.h"
#include "compile.h"
#include <memory>
#include <stdexcept>

using std::make_shared;
using std::runtime_error;

void mappings::Deserialize(JsonSerializer& js, std::shared_ptr<mappings::Mapping>& up)
{
    auto type = js.read<string>("_type");
    auto version = js.read<int>("_version");

    if (type == "Constant")
    {
        up = make_shared<compile::CompilableConstant>();
    }

    else if (type == "Sum")
    {
        up = make_shared<compile::CompilableSum>();
    }

    else if (type == "DecisionTreePath")
    {
        up = make_shared<compile::CompilableDecisionTreePath>();
    }

    else if (type == "Row")
    {
        up = make_shared<compile::CompilableRow>();
    }

    else if (type == "Column")
    {
        up = make_shared<compile::CompilableColumn>();
    }

    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }

    up->Deserialize(js, version);
}

