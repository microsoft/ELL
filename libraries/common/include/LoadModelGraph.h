////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelGraph.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "ModelGraph.h"

namespace common
{
    /// <summary> Loads a model from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="filename"> The filename. </param>
    /// <returns> The loaded model. </returns>
    model::Model LoadModelGraph(const std::string& filename);

    /// <summary> Saves a model to a file. </summary>
    ///
    /// <param name="model"> The model. </param>
    /// <param name="filename"> The filename. </param>
    /// <returns> The loaded model. </returns>
    void SaveModelGraph(const model::Model& model, const std::string& filename);
    
}
