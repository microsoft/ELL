// CoordinateListFactory.cpp

#include "CoordinateListFactory.h"

#include <memory>
using std::move;

namespace layers
{
    vector<Coordinate> CoordinateListFactory::Sequence(uint64 row, uint64 numColumns, uint64 fromColumn)
    {
        vector<Coordinate> coordinates;
        FillBack(coordinates, row, numColumns, fromColumn);
        return move(coordinates);
    }

    vector<Coordinate> CoordinateListFactory::IgnoreSuffix(const Map & map, uint64 numLayersToDrop)
    {
        uint64 row = map.NumLayers() - 1 - numLayersToDrop;
        uint64 numColumns = map.LayerSize(row);
        vector<Coordinate> coordinates;
        FillBack(coordinates, row, numColumns);
        return move(coordinates);
    }

    void CoordinateListFactory::FillBack(vector<Coordinate>& vector, uint64 row, uint64 numColumns, uint64 fromColumn)
    {
        for (uint64 column = fromColumn; column < fromColumn + numColumns; ++column)
        {
            vector.emplace_back(row, column);
        }
    }

}