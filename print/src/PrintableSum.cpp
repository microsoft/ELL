// PrintableSum.cpp

#include "PrintableSum.h"
#include "svgHelpers.h"

using std::make_unique;

#include <string>
using std::to_string;

uint64 PrintableSum::Size() const
{
    return Sum::Size();
}

string PrintableSum::GetTypeName() const
{
    return string("SUM");
}
