// MapLoaders.cpp

#include "MapLoaders.h"

// utilities
#include "files.h"

namespace common
{
    template<typename MapType>
    MapType GetMap(const std::string& inputMapFile)
    {
        // load map
        auto inputMapFStream = utilities::OpenIfstream(inputMapFile);
        return utilities::JsonSerializer::Load<MapType>(inputMapFStream, "Base");
    }
}
