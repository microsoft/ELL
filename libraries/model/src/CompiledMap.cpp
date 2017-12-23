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
    CompiledMap::CompiledMap(model::Map map, std::string functionName)
        : Map(std::move(map)), _functionName(functionName) {}
}
}
