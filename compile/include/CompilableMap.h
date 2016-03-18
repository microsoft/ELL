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
class CompilableMap : public layers::Stack
{
public:
    /// <summary> Generates C code that encodes the map. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="coordinateList"> List of output coordinates, whose values must be computed. </param>
    void ToCode(std::ostream& os, layers::CoordinateList coordinateList) const;
};
