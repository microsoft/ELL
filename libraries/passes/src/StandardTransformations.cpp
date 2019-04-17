////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StandardTransformations.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DetectLowPrecisionConvolutionTransformation.h"
#include "StandardTransformations.h"
#include "FuseLinearOperationsTransformation.h"
#include "OptimizeReorderDataNodesTransformation.h"
#include "SetConvolutionMethodTransformation.h"

#include <model/include/RefineTransformation.h>

#include <utilities/include/Exception.h>

namespace ell
{
using namespace model;
using namespace utilities;

namespace passes
{
    void AddStandardTransformationsToRegistry()
    {
        AddStandardTransformationsToRegistry(TransformationRegistry::GetGlobalRegistry());
    }

    void AddStandardTransformationsToRegistry(model::TransformationRegistry& registry)
    {
        static bool done = false;
        if (!done)
        {
            registry.AddTransformation<DetectLowPrecisionConvolutionTransformation>();
            registry.AddTransformation<SetConvolutionMethodTransformation>();
            registry.AddTransformation<model::RefineTransformation>();
            registry.AddTransformation<FuseLinearOperationsTransformation>();
            registry.AddTransformation<OptimizeReorderDataNodesTransformation>();
            done = true;
        }
    }
} // namespace passes
} // namespace ell
