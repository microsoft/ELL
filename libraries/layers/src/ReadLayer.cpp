////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     ReadLayer.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ReadLayer.h"
#include "Input.h"
#include "Coordinatewise.h"
#include "Sum.h"

namespace layers
{
    void Read(std::string runtimeTypeName, std::unique_ptr<Layer>& value)
    {
        if (runtimeTypeName == Input::GetTypeName())
        {
            value = std::make_unique<Input>();
        }
        else if (runtimeTypeName == Sum::GetTypeName())
        {
            value = std::make_unique<Sum>();
        }
        else if (runtimeTypeName == Coordinatewise::GetTypeName())
        {
            value = std::make_unique<Coordinatewise>(); 
        }
        else
        {
            throw std::runtime_error("attempted to deserialize an unrecognized class type");
        }
    }
}