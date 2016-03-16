////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     ConstructLayer.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"

// stl
#include <string>
#include <memory>

namespace layers
{
    /// <summary> Constructs a derived type of Layer according to the type name provided. </summary>
    ///
    /// <param name="runtimeTypeName"> Name of the type to construct. </param>
    /// <param name="value"> [in,out] Points to the newly constructed object. </param>
    void Construct(std::string runtimeTypeName, std::unique_ptr<Layer>& value);
}