////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueSelectorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "Node.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <exception>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class ValueSelectorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* conditionPortName = "condition";
        const model::InputPort<bool>& condition = _condition;
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        ValueSelectorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="condition"> An input that returns a single boolean value that selects which input to use as output </param>
        /// <param name="input1"> The input to return if the condition is `true` </param>
        /// <param name="input2"> The input to return if the condition is `false` </param>
        ValueSelectorNode(const model::PortElements<bool>& condition, const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ValueSelectorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        // Inputs
        model::InputPort<bool> _condition;
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
}
}

#include "../tcc/ValueSelectorNode.tcc"
