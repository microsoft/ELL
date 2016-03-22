////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableMap.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PrintArguments.h" 
#include "PrintableLayer.h"
#include "PrintableSum.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"

// layers
#include "Layer.h"
#include "Map.h"

// utilities
#include "TypeFactory.h"

// stl
#include <iostream>
#include <memory>

/// <summary> A printable map. </summary>
class PrintableMap : public layers::Map
{
public:

    PrintableMap(const layers::Map& other) : Map(other)
    {

        utilities::TypeFactory<PrintableLayer> printableLayerFactory;
        printableLayerFactory.AddType<PrintableInput>(layers::Input::GetTypeName());
        printableLayerFactory.AddType<PrintableCoordinatewise>(layers::Coordinatewise::GetTypeName());
        printableLayerFactory.AddType<PrintableSum>(layers::Sum::GetTypeName());

        for (const auto& layer : Map::_layers)
        {
            _layers.push_back(printableLayerFactory.Construct(layer->GetRuntimeTypeName()));
            (*_layers.back()) = (*layer);
        }
    }

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="Arguments"> The arguments. </param>
    void Print(std::ostream& os, const PrintArguments& arguments);

private:
    std::vector<std::unique_ptr<PrintableLayer>> _layers;
};
