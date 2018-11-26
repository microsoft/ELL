////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryFunctionNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/MapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <emitters/include/EmitterTypes.h>
#include <emitters/include/LLVMUtilities.h>

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a coordinatewise binary operation on its inputs ('binary' in the sense that there are 2 inputs). </summary>
    template <typename ValueType, typename FunctionType>
    class BinaryFunctionNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        BinaryFunctionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="function"> The function to apply coordinate-wise. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryFunctionNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, FunctionType function, ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="layout"> The layout for both inputs and the output. </param>
        /// <param name="function"> The function to apply coordinate-wise. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryFunctionNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, const model::PortMemoryLayout& inputLayout, FunctionType function, ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="inputLayout"> The layout for both inputs. </param>
        /// <param name="outputLayout"> The output layout. </param>
        /// <param name="function"> The function to apply coordinate-wise. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryFunctionNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType padding = 0);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, FunctionType>("BinaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        ell::utilities::ArchiveVersion GetArchiveVersion() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: paddingValue

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void ComputeDimensionLoop(size_t dimension,
                                  std::vector<ValueType>& output,
                                  size_t prevInputDimensionOffset,
                                  size_t prevOutputDimensionOffset) const;

        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                      emitters::IRFunctionEmitter& function,
                                      size_t dimension,
                                      emitters::LLVMValue input1,
                                      emitters::LLVMValue input2,
                                      emitters::LLVMValue output,
                                      emitters::LLVMValue prevInputDimensionOffset,
                                      emitters::LLVMValue prevOutputDimensionOffset) const;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;
        model::PortMemoryLayout _inputLayout;

        // Output
        model::OutputPort<ValueType> _output;

        // Function to apply coordinate-wise
        FunctionType _function;

        ValueType _paddingValue;
    };
} // namespace nodes
} // namespace ell

#include "../tcc/BinaryFunctionNode.tcc"