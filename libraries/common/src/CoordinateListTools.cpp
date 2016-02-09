////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CoordinateListTools.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateListTools.h"

namespace common
{
    layers::CoordinateList GetCoordinateList(const layers::Map& map, const CoordinateListLoadArguments& coordinateListLoadArguments)
    {
        return GetCoordinateListIgnoreMapSuffix(map, coordinateListLoadArguments.ignoreSuffix);
    }

    layers::CoordinateList GetCoordinateSequence(uint64 row, uint64 numColumns)
    {
        layers::CoordinateList coordinateList;
        for (uint64 column = 0; column < numColumns; ++column)
        {
            coordinateList.emplace_back(row, column);
        }
        return coordinateList;
    }

    layers::CoordinateList GetCoordinateListIgnoreMapSuffix(const layers::Map& map, uint64 numLayersToDrop)
    {
        return map.GetCoordinateList(map.NumLayers() - numLayersToDrop);
    }
}
