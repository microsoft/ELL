////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     LoadModel.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelLoadArguments.h"

//layers
#include "Model.h"

namespace common
{
    /// <summary> Loads a model from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="modelLoadArguments"> Model load arguments. </param>
    ///
    /// <returns> A unique pointer to the model. </returns>
    layers::Model LoadModel(const ModelLoadArguments& modelLoadArguments);
}
