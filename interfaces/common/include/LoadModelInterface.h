////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Our interface classes
#include "ModelInterface.h"

#include <string>

namespace interfaces
{
    /// <summary> Loads a model from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="filename"> Model filename. </param>
    ///
    /// <returns> A unique pointer to the model. </returns>
        interfaces::Model LoadModel(const std::string& filename);
}
