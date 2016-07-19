////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "OutputPort.h"
#include "ModelTransformer.h"

// utilities
#include "TypeName.h"

#include <vector>
#include <memory>
#include <string>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> A node that represents an input to the system. </summary>
    template <typename ValueType>
    class InputNode : public Node
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="dimension"> The input dimension </param>
        InputNode(size_t dimension);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The values for this node to output </param>
        void SetInput(std::vector<ValueType> inputValues);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("Input"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        std::vector<ValueType> _inputValues;
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/InputNode.tcc"
