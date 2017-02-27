////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegisterNodeCreators.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "ModelBuilder.h"

namespace ell
{
namespace common
{
    /// <summary> Registers node-creation functions with the ModelBuilder class </summary>
    void RegisterNodeCreators(model::ModelBuilder& builder);
}
}
