////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StandardTransformations.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/TransformationRegistry.h>

namespace ell
{
namespace passes
{
    /// <summary> Add standard transformations to the global transformation registry </summary>
    void AddStandardTransformationsToRegistry();

    /// <summary> Add standard transformations to a transformation registry </summary>
    void AddStandardTransformationsToRegistry(model::TransformationRegistry& registry);
} // namespace passes
} // namespace ell
