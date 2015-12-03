// PrintableShift.cpp

#include "PrintableShift.h"

void PrintableShift::Print(ostream & os, const vector<shared_ptr<IPrintable>>& layers) const
{
}

void PrintableShift::ComputeLayout(const CommandLineArgs& args, double yOffset)
{
}

Point PrintableShift::GetBeginPoint(uint64 index) const
{
    return Point();
}

Point PrintableShift::GetEndPoint(uint64 index) const
{
    return Point();
}
