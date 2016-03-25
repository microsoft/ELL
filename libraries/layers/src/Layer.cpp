////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Layer.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layer.h"
#include "Input.h"
#include "Coordinatewise.h"
#include "Sum.h"

namespace layers
{
    std::string Layer::GetTypeName()
    {
        return "Layer";
    }

    const utilities::TypeFactory<Layer> Layer::GetTypeFactory()
    {
        utilities::TypeFactory<Layer> factory;
        factory.AddType<Input>();
        factory.AddType<Coordinatewise>();
        factory.AddType<Sum>();
        return factory;
    }
}
