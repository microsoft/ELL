////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Stack.tcc (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "XMLSerialization.h"
#include "Files.h"

// stl
#include <string>

namespace layers
{
    template <typename LayerType>
    const LayerType& Stack::GetLayer(uint64 layerIndex) const
    {
        return dynamic_cast<const LayerType&>(*_layers[layerIndex]);
    }

    template<typename StackType>
    StackType Stack::Load(const std::string& inputStackFile)
    {
        auto inputMapFStream = utilities::OpenIfstream(inputStackFile);
        utilities::XMLDeserializer deserializer(inputMapFStream);

        StackType map;
        deserializer.Deserialize(map);
        return map;
    }

}
