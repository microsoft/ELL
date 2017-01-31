////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterException.h"
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"

namespace ell
{
namespace model
{
    void CompilableNode::Compile(MapCompiler& compiler)
    {
        auto irCompiler = dynamic_cast<IRMapCompiler*>(&compiler);
        if (irCompiler != nullptr)
        {
            Compile(*irCompiler);
            return;
        }

        throw emitters::EmitterException(emitters::EmitterError::notSupported, "Unknown compiler type");
    }
}
}
