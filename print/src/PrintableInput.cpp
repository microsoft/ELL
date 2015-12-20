// PrintableInput.cpp

#include "PrintableInput.h"

uint64 PrintableInput::Size() const
{
    return Input::Size();
}

string PrintableInput::GetTypeName() const
{
    return string("IN");
}
