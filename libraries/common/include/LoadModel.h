////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadModel.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MapLoadArguments.h"

#include <model/include/Map.h>
#include <model/include/Model.h>

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
} // namespace common
} // namespace ell

#pragma region implementation

#include <utilities/include/Archiver.h>
#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/JsonArchiver.h>

namespace ell
{
namespace common
{
    // STYLE internal use only from implementation, so not declared in main part of header file
    template <typename UnarchiverType>
    model::Map LoadArchivedMap(std::istream& stream)
    {
        try
        {
            utilities::SerializationContext context;
            RegisterNodeTypes(context);
            RegisterMapTypes(context);
            UnarchiverType unarchiver(stream, context);
            model::Map map;
            unarchiver.Unarchive(map);
            return map;
        }
        catch (const ell::utilities::Exception& ex)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read file: " + ex.GetMessage());
        }
    }
} // namespace common
} // namespace ell

#pragma endregion implementation
