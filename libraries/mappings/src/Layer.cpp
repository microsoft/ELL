// Layer.cpp

#include "Layer.h"

namespace mappings
{
    Layer::Layer(uint64 size) : _output(size)
    {}

    void Layer::Compute(const vector<unique_ptr<Layer>>& previousLayers)
    {}

    uint64 Layer::Size() const
    {
        return _output.size();
    }

    double Layer::GetValue(uint64 index) const
    {
        return _output[index];
    }

    void Layer::Clear()
    {
        std::fill(_output.begin(), _output.end(), 0);
    }
}