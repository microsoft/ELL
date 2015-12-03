// PrintableInput.cpp

#include "PrintableInput.h"

void PrintableInput::Print(ostream & os, const vector<shared_ptr<IPrintable>>& layers) const
{
}

void PrintableInput::ComputeLayout(const CommandLineArgs& args, double yOffset)
{
}

Point PrintableInput::GetBeginPoint(uint64 index) const
{
    return Point();
}

Point PrintableInput::GetEndPoint(uint64 index) const
{
    return Point();
}
