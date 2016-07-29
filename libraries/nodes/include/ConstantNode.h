////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantNode.h (nodes)
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

/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that contains a constant value. Has no inputs. </summary>
    template <typename ValueType>
    class ConstantNode : public model::Node
    {
    public:
        /// <summary> Constructor for a scalar constant </summary>
        ///
        /// <param name="value"> The scalar value </param>
        ConstantNode(ValueType value);

        /// Constructor for a vector constant
        ///
        /// <param name="value"> The vector value </param>
        ConstantNode(const std::vector<ValueType>& values);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ConstantNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the values contained in this node </summary>
        ///
        /// <returns> The values contained in this node </returns>
        const std::vector<ValueType>& GetValues() const { return _values; }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<ValueType>& output = _output;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        static constexpr char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        std::vector<ValueType> _values;
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/ConstantNode.tcc"
