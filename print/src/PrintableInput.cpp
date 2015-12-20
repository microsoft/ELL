// PrintableInput.cpp

#include "PrintableInput.h"

uint64 PrintableInput::Size() const
{
    return Zero::Size();
}

string PrintableInput::GetTypeName() const
{
    return string("IN");
}
