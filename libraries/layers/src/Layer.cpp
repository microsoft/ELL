////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Layer.cpp (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layer.h"
#include "Coordinatewise.h"
#include "Sum.h"

namespace layers
{
    const utilities::TypeFactory<Layer> Layer::GetTypeFactory()
    {
        utilities::TypeFactory<Layer> factory;
        factory.AddType<Coordinatewise>();
        factory.AddType<Sum>();
        return factory;
    }
}
