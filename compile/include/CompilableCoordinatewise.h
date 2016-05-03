////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompilableCoordinatewise.h (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Coordinatewise.h"

// stl
#include <cstdint>

/// <summary> A struct that adds printing capabilities to a layer. </summary>
class CompilableCoordinatewise : public layers::Coordinatewise, public CompilableLayer
{
public:
    /// <summary> Returns the output dimension of the layer. </summary>
    ///
    /// <returns> The output dimension. </returns>
    virtual uint64_t GetOutputDimension() const override;

    /// <summary> Assignment operator from Coordinatewise. </summary>
    ///
    /// <param name="coordinatewise"> The coordinatewise layer to assign from.</param>
    void operator=(const layers::Coordinatewise& coordinatewise);

    /// <summary> Assignment operator from Layer. </summary>
    ///
    /// <param name="layer"> The layer being copied. </param>
    virtual void operator=(const layers::Layer& layer) override;

    /// <summary> Pushes actions upward in the graph </summary>
    ///
    /// <param name="currentLayerIndex"> The index of the current layer. </param>
    /// <param name="graph"> [in,out] The data flow graph. </param>
    virtual void SetActions(uint64_t currentLayerIndex, DataFlowGraph& graph) const override;
};