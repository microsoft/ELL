////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MovingVarianceNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "ModelTransformer.h"
#include "OutputPortElements.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    /// <summary> A feature that takes a vector input and returns its variance over some window of time </summary>
    template <typename ValueType>
    class MovingVarianceNode : public model::Node
    {
    public:
        /// <summary> Input port name. </summary>
        static constexpr char* inputPortName = "input";

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the variance of </param>
        /// <param name="windowSize"> The number of samples of history to use in computing the variance </param>
        MovingVarianceNode(const model::OutputPortElements<ValueType>& input, size_t windowSize);
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MovingVarianceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<ValueType>& output = _output;

        /// <summary> Output port name. </summary>
        static constexpr char* outputPortName = "output";

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<std::vector<ValueType>> _samples;
        mutable std::vector<ValueType> _runningSum;
        mutable std::vector<ValueType> _runningSquaredSum;
        size_t _windowSize;
    };
}

#include "../tcc/MovingVarianceNode.tcc"