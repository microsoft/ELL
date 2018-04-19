////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledMap.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompiledMap.h"

namespace ell
{
namespace model
{
    CompiledMap::CompiledMap(model::Map map, std::string functionName, const MapCompilerOptions& options)
        : Map(std::move(map)), _functionName(functionName), _compilerOptions(options) {}
}
}
