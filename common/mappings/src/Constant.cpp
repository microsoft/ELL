// Constant.cpp

#include "Constant.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::to_string;

namespace mappings
{
    Constant::Constant(double c) : _c(c)
    {
        _type = types::Constant;
    }

    void Constant::SetConstant(double c)
    {
        _c = c;
    }

    double Constant::GetConstant() const
    {
        return _c;
    }

    void Constant::apply(const double* input, double* output) const
    {
        output[0] = _c;
    }

    int Constant::GetMinInputDim() const
    {
        return 0;
    }

    int Constant::GetOutputDim() const
    {
        return 1;
    }

    void Constant::Serialize(JsonSerializer& js) const
    {
        // version 1
        Mapping::SerializeHeader(js, 1);
        js.write("c", _c);
    }

    void Constant::Deserialize(JsonSerializer& js, int version)
    {
        if (version == 1)
        {
            js.read("c", _c);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
