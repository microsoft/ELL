////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledMap.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompiledMap.h"
#include "EmitterException.h"

// model
#include "ModelTransformer.h"
#include "OutputNode.h"
#include "Port.h"

// utilities
#include "Exception.h"
#include "Files.h"

// stl
#include <sstream>

namespace ell
{
namespace model
{
    CompiledMap::CompiledMap()
    {
    }

    CompiledMap::CompiledMap(const model::Model& model, const std::vector<std::pair<std::string, model::InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, model::PortElementsBase>>& outputs)
        : DynamicMap(model, inputs, outputs), _functionName("predict"), _optimize(true)
    {
    }

    CompiledMap::CompiledMap(const model::DynamicMap& other, const std::string& functionName, bool optimize)
        : DynamicMap(other), _functionName(functionName), _optimize(optimize)
    {
    }

    void CompiledMap::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::DynamicMap::WriteToArchive(archiver);
    }

    void CompiledMap::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::DynamicMap::ReadFromArchive(archiver);
        Compile();
    }
}
}
