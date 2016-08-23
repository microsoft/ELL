////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryPredicateNode.h (features)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// utilities
#include "Exception.h"
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    /// <summary> A node that performs a coordinatewise binary boolean-valued operation on its inputs. </summary>
    template <typename ValueType>
    class BinaryPredicateNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* input1PortName = "input1";
        static constexpr const char* input2PortName = "input2";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<bool>& output = _output;
        /// @}

        /// <summary> Types of coordinatewise operations supported by this node type. </summary>
        enum class PredicateType
        {
            none,
            equal,
            less,
            greater,
            notEqual,
            lessOrEqual,
            greaterOrEqual
        };

        /// <summary> Default Constructor </summary>
        BinaryPredicateNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the arithmetic expression. </param>
        /// <param name="input2"> The right-hand input of the arithmetic expression. </param>
        /// <param name="predicate"> The type of predicate to apply. </param>
        BinaryPredicateNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, PredicateType predicate);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryPredicateNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        template <typename Operation>
        std::vector<bool> ComputeOutput(Operation&& fn) const;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<bool> _output;

        // Operation
        PredicateType _predicate;
    };
}

#include "../tcc/BinaryPredicateNode.tcc"