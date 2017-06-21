////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadModel.tcc (common)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Archiver.h"
#include "Files.h"
#include "JsonArchiver.h"

namespace ell
{
namespace common
{
    // STYLE internal use only from .tcc, so not declared inside header file
    template <typename UnarchiverType, typename MapType>
    MapType LoadArchivedMap(std::istream& stream)
    {
        try
        {
            utilities::SerializationContext context;
            RegisterNodeTypes(context);
            RegisterMapTypes(context);
            UnarchiverType unarchiver(stream, context);
            MapType map;
            unarchiver.Unarchive(map);
            return map;
        }
        catch (const std::exception&)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read file.");
        }
    }

    template <typename MapType>
    MapType LoadMap(const std::string& filename)
    {
        if (filename == "")
        {
            return MapType{};
        }

        if (!utilities::IsFileReadable(filename))
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound);
        }

        auto filestream = utilities::OpenIfstream(filename);
        return LoadArchivedMap<utilities::JsonUnarchiver, MapType>(filestream);
    }

    template <typename MapType, MapLoadArguments::MapType argMapType>
    MapType LoadMap(const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.HasMapFilename())
        {
            if (mapLoadArguments.mapType == argMapType)
            {
                return common::LoadMap<MapType>(mapLoadArguments.inputMapFilename);
            }
            else
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: map type does not match.");
            }
        }
        else
        {
            // Short circuiting the generic implementation
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: missing map filename.");
        }
    }

    // STYLE Specialization for DynamicMap declared here to avoid multiply defined symbols when the .cpp is not part of compilation unit
    template <>
    model::DynamicMap LoadMap<model::DynamicMap, MapLoadArguments::MapType::simpleMap>(const MapLoadArguments& mapLoadArguments);
}
}