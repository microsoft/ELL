// Constant.cpp

#include "Constant.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Constant::Constant(double c) : _c(c)
    {
        _type = Types::constant;
    }

    void Constant::SetConstant(double c)
    {
        _c = c;
    }

    double Constant::GetConstant() const
    {
        return _c;
    }

    void Constant::Apply(const double* input, double* output) const
    {
        output[0] = _c;
    }

    uint64 Constant::GetMinInputDim() const
    {
        return 0;
    }

    uint64 Constant::GetOutputDim() const
    {
        return 1;
    }

    void Constant::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("c", _c);
    }

    void Constant::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("c", _c);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
