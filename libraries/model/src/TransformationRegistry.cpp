////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformationRegistry.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformationRegistry.h"

namespace ell
{
namespace model
{
    TransformationRegistry& TransformationRegistry::GetGlobalRegistry()
    {
        static TransformationRegistry registry;
        return registry;
    }
} // namespace model
} // namespace ell
