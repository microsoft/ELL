////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     ConstructLayer.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableInput.h"
#include "CompilableCoordinatewise.h"
#include "CompilableSum.h"

namespace layers
{
    void Construct(std::string runtimeTypeName, std::unique_ptr<Layer>& value)
    {
        if (runtimeTypeName == Input::GetTypeName())
        {
            value = std::make_unique<CompilableInput>();
        }
        else if (runtimeTypeName == Sum::GetTypeName())
        {
            value = std::make_unique<CompilableSum>();
        }
        else if (runtimeTypeName == Coordinatewise::GetTypeName())
        {
            value = std::make_unique<CompilableCoordinatewise>();
        }
        else
        {
            throw std::runtime_error("attempted to deserialize an unrecognized class type");
        }
    }
}