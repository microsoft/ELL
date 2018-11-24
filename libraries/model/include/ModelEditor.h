////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelEditor.h (model)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "OutputPort.h"

namespace ell
{
namespace model
{
    /// <summary> Helper class to provide operations that edit various aspects of a model </summary>
    class ModelEditor
    {
    public:
        /// <summary> Reset the input to an input port for a node </summary>
        /// <param name="port"> The input port that should have its input reset </param>
        /// <param name="newInput"> The new input for the input port </param>
        /// <remarks> This should ideally only be used sparingly, such as during the optimization process </remarks>
        static void ResetInputPort(const InputPortBase* port, const OutputPortBase& newInput);
    };
} // namespace model
} // namespace ell