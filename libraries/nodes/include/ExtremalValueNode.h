////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExtremalValueNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNodeUtilities.h"
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "Node.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> An example node that computes an extremal value (min or max) of its input, as well as the index of the extremal value. </summary>
    template <typename ValueType, bool max>
    class ExtremalValueNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* valPortName = "val";
        static constexpr const char* argValPortName = "argVal";

        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& val = _val;
        const model::OutputPort<int>& argVal = _argVal;
        /// @}

        /// <summary> Default Constructor </summary>
        ExtremalValueNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ExtremalValueNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this is an argmin or argmax node </summary>
        ///
        /// <returns> `true` if this is an argmax node, `false` if is an argmin node </returns>
        bool IsMaxNode() const { return max; }

        /// <summary> Gets the emitter typed comparison type used for computing this node </summary>
        ///
        /// <returns> A `TypedComparison` indicating the comarison type and data type for this node </returns>
        emitters::TypedComparison GetComparison() const;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        llvm::Function* GetOperator(model::IRMapCompiler& compiler) const;
        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // My inputs
        model::InputPort<ValueType> _input;

        // My outputs
        model::OutputPort<ValueType> _val;
        model::OutputPort<int> _argVal;
    };

    /// <summary> ArgMin node subclass </summary>
    template <typename ValueType>
    class ArgMinNode : public ExtremalValueNode<ValueType, false>
    {
    public:
        /// <summary> Default Constructor </summary>
        ArgMinNode()
            : ExtremalValueNode<ValueType, false>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMinNode(const model::PortElements<ValueType>& input)
            : ExtremalValueNode<ValueType, false>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMinNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;
    };

    /// <summary> ArgMax node subclass </summary>
    template <typename ValueType>
    class ArgMaxNode : public ExtremalValueNode<ValueType, true>
    {
    public:
        /// <summary> Default Constructor </summary>
        ArgMaxNode()
            : ExtremalValueNode<ValueType, true>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMaxNode(const model::PortElements<ValueType>& input)
            : ExtremalValueNode<ValueType, true>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMaxNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;
    };
}
}

#include "../tcc/ExtremalValueNode.tcc"
