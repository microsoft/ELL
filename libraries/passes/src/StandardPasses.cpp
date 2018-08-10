////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StandardPasses.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FuseLinearOperationsPass.h"
#include "OptimizeReorderDataNodes.h"
#include "SetConvolutionMethodPass.h"

// model
#include "OutputNode.h"

// utilities
#include "Exception.h"

namespace ell
{

using namespace model;
using namespace utilities;

namespace passes
{
    void AddStandardPassesToRegistry()
    {
        SetConvolutionMethodPass::AddToRegistry();
        FuseLinearOperationsPass::AddToRegistry();
        OptimizeReorderDataNodes::AddToRegistry();
    }
}
}
