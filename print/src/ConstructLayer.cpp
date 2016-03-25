////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     ConstructLayer.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"

namespace layers
{
    void Construct(std::string runtimeTypeName, std::unique_ptr<Layer>& value)
    {
        if (runtimeTypeName == Input::GetTypeName())
        {
            value = std::make_unique<PrintableInput>();
        }
        else if (runtimeTypeName == Sum::GetTypeName())
        {
            value = std::make_unique<PrintableSum>();
        }
        else if (runtimeTypeName == Coordinatewise::GetTypeName())
        {
            value = std::make_unique<PrintableCoordinatewise>();
        }
        else
        {
            throw std::runtime_error("attempted to deserialize an unrecognized class type");
        }
    }
}