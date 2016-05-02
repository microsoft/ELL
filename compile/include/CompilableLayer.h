////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompilableLayer.h (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataFlowGraph.h"

// stl
#include <cstdint>
#include <vector>
#include <string>

/// <summary> An interface for classes that can print human-friendly descriptions of themselves. </summary>
class CompilableLayer 
{
public:
    virtual ~CompilableLayer() = default;

    /// <summary> Returns the number of elements in the layer. </summary>
    ///
    /// <returns> The number of elements in the layer. </returns>
    virtual uint64_t Size() const = 0;

    /// <summary> Pushes actions upward in the graph </summary>
    ///
    /// <param name="currentLayerIndex"> The index of the current layer. </param>
    /// <param name="graph"> [in,out] The data flow graph. </param>
    virtual void SetActions(uint64_t currentLayerIndex, DataFlowGraph& graph) const = 0;

    /// <summary> Assignment operator from layer to CompilableLayer. </summary>
    ///
    /// <param name="layer"> The layer being copied. </param>
    virtual void operator=(const layers::Layer& layer) = 0;

    /// <summary> Gets the name of this type. </summary>
    ///
    /// <returns> The name of this type. </returns>
    static std::string GetTypeName();
};

 