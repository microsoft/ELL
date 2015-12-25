// Compiler.cpp

#include "Compiler.h"
#include "CompilableInput.h"
#include "CompilableScale.h"
#include "CompilableShift.h"
#include "CompilableSum.h"

using std::make_shared;

#include <string>
using std::to_string;

#include <memory>
using std::dynamic_pointer_cast;

void Compiler::Deserialize(JsonSerializer & serializer)
{
    serializer.Read("layers", _layers, Compiler::DeserializeLayers);
}

void Compiler::DeserializeLayers(JsonSerializer & serializer, shared_ptr<Compilable>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Input")
    {
        auto upZero = make_shared<CompilableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = make_shared<CompilableScale>();
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<CompilableShift>();
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = make_shared<CompilableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }
}

void Compiler::SetActions()
{
    if (_layers.size() == 0)
    {
        return;
    }

    // set trivial actions for output elements (actually, do this in a separate fiunction called SetOutputs)

    // iterate over layers from the bottom up
    for (uint64 i = _layers.size() - 1; i > 0; ++i)
    {
        // _layers[i]->SetActions(_layers);
    }
}

