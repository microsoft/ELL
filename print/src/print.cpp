// print.cpp

#include "print.h"
#include <memory>
using std::dynamic_pointer_cast;
using std::make_shared;

using std::endl;

void PrintableMap::Print(ostream & os)
{
    for (uint64 k = 0; k < _printables.size(); ++k)
    {
        _printables[k]->Print(os);
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
        auto upZero = make_shared<PrintableZero>();
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

void PrintableZero::Print(ostream& os)
{
    os << "Zero\tsize: " << Size() << endl;
}

void PrintableScale::Print(ostream& os)
{
    os << "Scale\tsize: " << Size() << endl;
}

void PrintableShift::Print(ostream& os)
{
    os << "Shift\tsize: " << Size() << endl;
}

void PrintableSum::Print(ostream& os)
{
    os << "Sum\tsize: " << Size() << endl;
}

