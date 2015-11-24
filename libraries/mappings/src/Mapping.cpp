// Mapping.cpp

#include "Mapping.h"

namespace mappings
{
    void Mapping::Apply(const vector<double>& input, vector<double>& output) const
    {
        Apply(&input[0], &output[0]);
    }

    Mapping::Types Mapping::GetType() const
    {
        return _type;
    }

    void Mapping::SerializeHeader(JsonSerializer& serializer, int version) const
    {
        // to make the file more readable
        const string type_names[] = { "constant", "scale", "shift", "sum", "decisionTreePath", "row", "column" };
        serializer.Write("_type", type_names[(int)_type]);
        serializer.Write("_version", version);
    }
}