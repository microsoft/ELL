////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MagnitudeNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "ModelTransformer.h"
#include "OutputPortElementList.h"
#include "InputPort.h"
#include "OutputPort.h"

// stl
#include <string>
#include <cmath>

namespace nodes
{
    /// <summary> A feature that takes a vector input and returns its magnitude </summary>
    template <typename ValueType>
    class MagnitudeNode : public model::Node
    {
    public:
        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the magnitude of </param>
        MagnitudeNode(const model::OutputPortElementList<ValueType>& input);
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "MagnitudeNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<ValueType>& output = _output;

        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/MagnitudeNode.tcc"