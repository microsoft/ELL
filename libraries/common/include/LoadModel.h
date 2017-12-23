////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadModel.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MapLoadArguments.h"

// model
#include "Map.h"
#include "Model.h"

namespace ell
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

    /// <summary> Saves a model to a stream. </summary>
    ///
    /// <param name="model"> The model. </param>
    /// <param name="outStream"> The stream. </param>
    void SaveModel(const model::Model& model, std::ostream& outStream);

    /// <summary> Register known node types to a serialization context </summary>
    ///
    /// <param name="context"> The `SerializationContext` </param>
    void RegisterNodeTypes(utilities::SerializationContext& context);

    /// <summary> Register known map types to a serialization context </summary>
    ///
    /// <param name="context"> The `SerializationContext` </param>
    void RegisterMapTypes(utilities::SerializationContext& context);

    /// <summary> Loads a map from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="filename"> The filename. </param>
    /// <returns> The loaded map. </returns>
    model::Map LoadMap(const std::string& filename);

    /// <summary> Loads a map from a `MapLoadArguments` struct. </summary>
    ///
    /// <param name="mapLoadArguments"> The `MapLoadArguments` struct. </param>
    /// <returns> The loaded map. </returns>
    model::Map LoadMap(const MapLoadArguments& mapLoadArguments);

    /// <summary> Saves a map to a file. </summary>
    ///
    /// <param name="map"> The map. </param>
    /// <param name="filename"> The filename. </param>
    void SaveMap(const model::Map& map, const std::string& filename);

    /// <summary> Saves a map to a stream. </summary>
    ///
    /// <param name="map"> The map. </param>
    /// <param name="outStream"> The stream. </param>
    void SaveMap(const model::Map& map, std::ostream& outStream);
}
}

#include "../tcc/LoadModel.tcc"