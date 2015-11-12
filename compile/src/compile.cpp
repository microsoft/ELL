// compile.cpp

#include "compile.h"
#include <memory>

using std::endl;
using std::dynamic_pointer_cast;

namespace compile
{
    void indent(ostream& os, int indentation)
    {
        for (int i = 0; i < indentation; ++i)
        {
            os << "\t";
        }
    }

    void CompilableConstant::compile(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "CONSTANT\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void CompilableSum::compile(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "SUM\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void CompilableDecisionTreePath::compile(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "DECISION TREE PATH\tmin input dim: " << GetMinInputDim() << ", output dim: " << GetOutputDim() << endl;
    }

    void CompilableRow::compile(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "ROW\tmin input dim : " << GetMinInputDim() << ", output dim : " << GetOutputDim() << ", width: " << _row_elements.size() << endl;

        auto begin = _row_elements.cbegin();
        auto end = _row_elements.cend();
        int index = 1;
        while (begin != end)
        {
            auto compilable_map = dynamic_pointer_cast<compilable>(*begin);
            compilable_map->compile(os, indentation + 1, index++);
            ++begin;
        }
    }

    void CompilableColumn::compile(ostream& os, int indentation, int enumeration)
    {
        indent(os, indentation);
        if (indentation > 0)
        {
            os << enumeration << ". ";
        }
        os << "COLUMN\tmin input dim : " << GetMinInputDim() << ", output dim : " << GetOutputDim() << ", height: " << _column_elements.size() << endl;

        auto begin = _column_elements.cbegin();
        auto end = _column_elements.cend();
        int index = 1;
        while (begin != end)
        {
            auto compilable_map = dynamic_pointer_cast<compilable>(*begin);
            compilable_map->compile(os, indentation + 1, index++);
            ++begin;
        }
    }
}