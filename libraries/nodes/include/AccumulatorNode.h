////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AccumulatorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNodeUtilities.h"
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that accumulates a running sum of its input. </summary>
    template <typename ValueType>
    class AccumulatorNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        AccumulatorNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to accumulate </param>
        AccumulatorNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("AccumulatorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual bool HasState() const override { return true; }
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* accumulator);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* accumulator);

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<ValueType> _accumulator;
    };
}
}

#include "../tcc/AccumulatorNode.tcc"