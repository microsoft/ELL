////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModel.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"

namespace emll
{
namespace common
{
    /// <summary> Loads a model from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="filename"> The filename. </param>
    /// <returns> The loaded model. </returns>
    model::Model LoadModel(const std::string& filename);

    /// <summary> Saves a model to a file. </summary>
    ///
    /// <param name="model"> The model. </param>
    /// <param name="filename"> The filename. </param>
    void SaveModel(const model::Model& model, const std::string& filename);

    /// <summary> Register known node types to a serialization context </summary>
    ///
    /// <param name="context"> The `SerializationContext` </param>
    void RegisterNodeTypes(utilities::SerializationContext& context);
}
}
