////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompilableSum.h (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Sum.h"

// stl
#include <cstdint>

/// <summary> A struct that adds printing capabilities to a layer. </summary>
class CompilableSum : public layers::Sum, public CompilableLayer
{
public:
    /// <summary> Returns the output dimension of the layer. </summary>
    ///
    /// <returns> The output dimension. </returns>
    virtual uint64_t GetOutputDimension() const override { return Sum::GetOutputDimension(); }

    /// <summary> Assignment operator from Sum. </summary>
    ///
    /// <param name="sum"> The Sum layer to assign from.</param>
    void operator=(const layers::Sum& sum);

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