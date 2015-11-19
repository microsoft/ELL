// Mapping.cpp

#include "Mapping.h"

namespace mappings
{
    void Mapping::Apply(const vector<double>& input, vector<double>& output) const
    {
        Apply(&input[0], &output[0]);
    }

    Mapping::types Mapping::GetType() const
    {
        return _type;
    }

    void Mapping::SerializeHeader(JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        const string type_names[] = { "Constant", "Scale", "Shift", "Sum", "DecisionTreePath", "Row", "Column" };
        serializer.Write("_type", type_names[(int)_type]);
        serializer.Write("_version", version);
    }
}