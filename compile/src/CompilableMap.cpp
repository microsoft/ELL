// CompilableMap.cpp

#include "CompilableMap.h"
#include "CompilableInput.h"
#include "CompilableScale.h"
#include "CompilableShift.h"
#include "CompilableSum.h"

using std::make_shared;

#include <string>
using std::to_string;

#include <memory>
using std::dynamic_pointer_cast;

#include <stdexcept>
using std::runtime_error;

void CompilableMap::ToCode(CoordinateList coordinateList) const
{
    // allocate datastructure to hold actions
    vector<vector<vector<AddToAction>>> actions(NumLayers());
    for(uint64 layerIndex = 0; layerIndex < NumLayers(); ++layerIndex)
    {
        actions[layerIndex].resize(_layers[layerIndex]->Size());
    }

    // backwards pass to assign actions
    for(uint64 layerIndex = NumLayers() - 1; layerIndex > 0; --layerIndex)
    {
        auto compilableLayer = GetLayer<CompilableLayer>(layerIndex);
        compilableLayer->BackwardPass(layerIndex, actions);
    }

    // forwards pass, to output code
    for(uint64 layer = 0; layer < NumLayers(); ++layer)
    { }

}

void CompilableMap::Deserialize(JsonSerializer& serializer)
{
    serializer.Read("layers", _layers, CompilableMap::DeserializeLayers);
}

void CompilableMap::DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Input")
    {
        auto upZero = make_shared<CompilableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = make_shared<CompilableScale>();
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<CompilableShift>();
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = make_shared<CompilableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }
}
