// CoordinateListFactory.cpp

#include "CoordinateListFactory.h"

namespace common
{
    CoordinateList GetCoordinateList(const CoordinateListLoadArguments & coordinateListLoadArguments)
    {
        return CoordinateList();
    }

    CoordinateList CoordinateSequence(uint64 row, uint64 numColumns)
    {
        CoordinateList coordinateList;
        for (uint64 column = 0; column < numColumns; ++column)
        {
            coordinateList.emplace_back(row, column);
        }
        return coordinateList;
    }

    CoordinateList CoordinateListIgnoreMapSuffix(const Map& map, uint64 numLayersToDrop)
    {
        return map.GetCoordinateList(map.NumLayers() - numLayersToDrop);
    }
}