////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DotProductNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> A feature that takes two vector inputs and returns their dot product </summary>
    template <typename ValueType>
    class DotProductNode : public model::Node
    {
    public:
        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to take the dot product of </param>
        DotProductNode(const model::OutputPortElementList<ValueType>& input1, const model::OutputPortElementList<ValueType>& input2);
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "DotProductNode"; }

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
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/DotProductNode.tcc"