// print.cpp

#include "print.h"
#include <memory>

using std::endl;
using std::dynamic_pointer_cast;

namespace print
{
    void indent(ostream& os, int indentation)
    {
        for (int i = 0; i < indentation; ++i)
        {
            os << "\t";
        }
    }

    void PrintableConstant::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "CONSTANT\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void PrintableScale::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if(indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "SCALE\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void PrintableShift::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if(indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "SHIFT\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void PrintableSum::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "SUM\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void PrintableDecisionTreePath::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "DECISION TREE PATH\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void PrintableRow::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "ROW\tmin input dim : " << GetMinInputDim() << ", output dim : " << GetOutputDim() << ", width: " << _rowElements.size() << endl;

        auto begin = _rowElements.cbegin();
        auto end = _rowElements.cend();
        int index = 1;
        while (begin != end)
        {
            auto printable_map = dynamic_pointer_cast<printable>(*begin);
            printable_map->Print(os, indentation + 1, index++);
            ++begin;
        }
    }

    void PrintableColumn::Print(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "COLUMN\tmin input dim : " << GetMinInputDim() << ", output dim : " << GetOutputDim() << ", height: " << _columnElements.size() << endl;

        auto begin = _columnElements.cbegin();
        auto end = _columnElements.cend();
        int index = 1;
        while (begin != end)
        {
            auto printable_map = dynamic_pointer_cast<printable>(*begin);
            printable_map->Print(os, indentation + 1, index++);
            ++begin;
        }
    }
}