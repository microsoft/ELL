// PrintableScale.cpp

#include "PrintableScale.h"

#include <memory>
using std::make_unique;

void PrintableScale::Print(ostream & os, const vector<shared_ptr<IPrintable>>& layers) const
{
}

void PrintableScale::ComputeLayout(double xOffset, double yOffset, double xMax)
{
    _upLayout = make_unique<LayerLayout>(xOffset, yOffset, xMax, 100);
}

Point PrintableScale::GetBeginPoint(uint64 index) const
{
    return Point();
}

Point PrintableScale::GetEndPoint(uint64 index) const
{
    return Point();
}
