#include "Zero.h"

mappings::Zero::Zero(uint64 size) : Layer(size)
{}

void mappings::Zero::Compute(const vector<unique_ptr<Layer>>& previousLayers)
{
    Clear();
}

void mappings::Zero::Serialize(JsonSerializer & serializer) const
{
    // TODO serialize the size
}

void mappings::Zero::Deserialize(JsonSerializer & serializer, int version)
{
}
