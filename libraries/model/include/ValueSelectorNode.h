////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <vector>
#include <memory>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class ValueSelectorNode : public Node
    {
    public:
        /// <summary> Default Constructor </summary>
        ValueSelectorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="condition"> An input that returns a single boolean value that selects which input to use as output </param>
        /// <param name="input1"> The input to return if the condition is `true` </param>
        /// <param name="input2"> The input to return if the condition is `false` </param>
        ValueSelectorNode(const OutputPortElements<bool>& condition, const OutputPortElements<ValueType>& input1, const OutputPortElements<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ValueSelectorNode"); }

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

        static constexpr const char* conditionPortName = "condition";
        static constexpr const char* input1PortName = "input1";
        static constexpr const char* input2PortName = "input2";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        InputPort<bool> _condition;
        InputPort<ValueType> _input1;
        InputPort<ValueType> _input2;

        // Output
        OutputPort<ValueType> _output;
    };

    //
    // Crud to throw away
    //

    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class SelectIfLessNode : public Node
    {
    public:
        SelectIfLessNode(const OutputPortElements<ValueType>& input1, const OutputPortElements<ValueType>& input2, const OutputPortElements<ValueType>& value1, const OutputPortElements<ValueType>& value2);
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SelectIfLessNode"); }
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        const OutputPort<ValueType>& output = _output;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(ModelTransformer& transformer) const override;

        static constexpr const char* input1PortName = "input1";
        static constexpr const char* input2PortName = "input2";
        static constexpr const char* value1PortName = "value1";
        static constexpr const char* value2PortName = "value2";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        InputPort<ValueType> _input1;
        InputPort<ValueType> _input2;
        InputPort<ValueType> _value1;
        InputPort<ValueType> _value2;

        // Output
        OutputPort<ValueType> _output;
    };

    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class IfLessNode : public Node
    {
    public:
        IfLessNode(const OutputPortElements<ValueType>& value, const OutputPortElements<ValueType>& threshold);
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("IfLessNode"); }
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        const OutputPort<bool>& output = _output;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(ModelTransformer& transformer) const override;

        static constexpr const char* valuePortName = "value";
        static constexpr const char* thresholdPortName = "threshold";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        InputPort<ValueType> _value;
        InputPort<ValueType> _threshold;

        // Output
        OutputPort<bool> _output;
    };
}

#include "../tcc/ValueSelectorNode.tcc"
