// Coordinate.cpp

#include "Coordinate.h"

namespace layers
{
    Coordinate::Coordinate(uint64 row, uint64 column) : _row(row), _column(column)
    {}

    uint64 Coordinate::GetRow() const
    {
        return _row;
    }

    uint64 Coordinate::GetColumn() const
    {
        return _column;
    }

    void Coordinate::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("r", _row); 
        serializer.Write("c", _column);
    }

    void Coordinate::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("r", _row);
        serializer.Read("c", _column);
    }
}