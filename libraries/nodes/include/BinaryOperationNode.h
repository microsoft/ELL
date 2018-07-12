////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryOperationNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "PortMemoryLayout.h"

// emitters
#include "EmitterTypes.h"

// utilities
#include "ArchiveVersion.h"
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
    /// <summary> A node that performs a coordinatewise binary arithmetic operation on its inputs. </summary>
    template <typename ValueType>
    class BinaryOperationNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        BinaryOperationNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the arithmetic expression. </param>
        /// <param name="input2"> The right-hand input of the arithmetic expression. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use the same memory layout as input1
        BinaryOperationNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, emitters::BinaryOperationType operation);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="layout"> The memory layout for the inputs and outputs. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryOperationNode(const model::PortElements<ValueType>& input1,
                            const model::PortElements<ValueType>& input2,
                            const model::PortMemoryLayout& layout,
                            emitters::BinaryOperationType operation,
                            ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="inputLayout1"> The layout for the left-hand input. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="inputLayout2"> The layout for the right-hand input. </param>
        /// <param name="outputLayout"> The output layout. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryOperationNode(const model::PortElements<ValueType>& input1,
                            const model::PortMemoryLayout& inputLayout1,
                            const model::PortElements<ValueType>& input2,
                            const model::PortMemoryLayout& inputLayout2,
                            const model::PortMemoryLayout& outputLayout,
                            emitters::BinaryOperationType operation,
                            ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Gets the operation performed by this node </summary>
        ///
        /// <returns> The operation </returns>
        emitters::BinaryOperationType GetOperation() const { return _operation; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: operation

    private:
        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                      emitters::IRFunctionEmitter& function,
                                      size_t dimension,
                                      llvm::Value* input1,
                                      llvm::Value* input2,
                                      llvm::Value* output,
                                      llvm::Value* prevInput1DimensionOffset,
                                      llvm::Value* prevInput2DimensionOffset,
                                      llvm::Value* prevOutputDimensionOffset) const;
        template <typename Operation>
        std::vector<ValueType> ComputeOutput(Operation&& function) const;
        template <typename Operation>
        void ComputeDimensionLoop(Operation& function,
                                  size_t dimension,
                                  std::vector<ValueType>& output,
                                  size_t prevInput1DimensionOffset,
                                  size_t prevInput2DimensionOffset,
                                  size_t prevOutputDimensionOffset) const;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::PortMemoryLayout _inputLayout1;
        model::InputPort<ValueType> _input2;
        model::PortMemoryLayout _inputLayout2;

        // Output
        model::OutputPort<ValueType> _output;

        // Operation
        emitters::BinaryOperationType _operation;

        // Padding
        ValueType _paddingValue;
    };
}
}

#include "../tcc/BinaryOperationNode.tcc"