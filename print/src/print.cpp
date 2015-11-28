// print.cpp

#include "print.h"
#include <memory>

using std::endl;
using std::dynamic_pointer_cast;

void indent(ostream& os, int indentation)
{
    for (int i = 0; i < indentation; ++i)
    {
        os << "\t";
    }
}

void PrintableZero::Print(ostream& os, int indentation, int enumeration)
{
    indent(os, indentation);
    if (indentation > 0)
    {
        os << enumeration << ". ";
    }
    os << "Zero\tsize: " << Size() << endl;
}

void PrintableScale::Print(ostream& os, int indentation, int enumeration)
{
    indent(os, indentation);
    if(indentation > 0)
    {
        os << enumeration << ". ";
    }
    os << "Scale\tsize: " << Size() << endl;
}

void PrintableShift::Print(ostream& os, int indentation, int enumeration)
{
    indent(os, indentation);
    if(indentation > 0)
    {
        os << enumeration << ". ";
    }
    os << "Shift\tsize: " << Size() << endl;
}

void PrintableSum::Print(ostream& os, int indentation, int enumeration)
{
    indent(os, indentation);
    if (indentation > 0)
    {
        os << enumeration << ". ";
    }
    os << "Sum\tsize: " << Size() << endl;
}
