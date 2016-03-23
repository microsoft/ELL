////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableMap.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// stl
#include <memory>
#include <iostream>

/// <summary> A compilable map. </summary>
class CompilableMap 
{
public:
    /// <summary> Copy constructor that constructs an instance of CompilableMap from a Map. </summary>
    ///
    /// <param name="other"> The Map being copied. </param>
    CompilableMap(const layers::Map& other);

    /// <summary> Generates C code that encodes the map. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="coordinateList"> List of output coordinates, whose values must be computed. </param>
    void ToCode(std::ostream& os, layers::CoordinateList coordinateList) const;

private:
    std::vector<std::unique_ptr<CompilableLayer>> _compilableLayers;
};
