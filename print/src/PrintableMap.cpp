// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableShift.h"
#include "PrintableInput.h"
#include "PrintableScale.h"
#include "PrintableSum.h"

using std::make_shared;

void PrintableMap::Print(ostream & os)
{
    for (uint64 k = 0; k < _printables.size(); ++k)
    {
        _printables[k]->Print(os, _printables);
    }
}

void PrintableMap::Deserialize(JsonSerializer & serializer)
{
    serializer.Read("layers", _printables, PrintableMap::DeserializeLayers);
}

void PrintableMap::DeserializeLayers(JsonSerializer & serializer, shared_ptr<IPrintable>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Zero")
    {
        auto upZero = make_shared<PrintableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = make_shared<PrintableScale>();
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<PrintableShift>();
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = make_shared<PrintableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }
}

