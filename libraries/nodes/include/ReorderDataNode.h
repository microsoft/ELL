////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReorderDataNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "Port.h"
#include "PortMemoryLayout.h"

// utilities
#include "IArchivable.h"

// stl
#include <algorithm>
#include <array>
#include <numeric>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that can reorder dimensions (transpose) and add or remove padding </summary>
    template <typename ValueType>
    class ReorderDataNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReorderDataNode();

        /// <summary> Constructor with no reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input. Only data in the "active" area will be copied. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue = 0);

        /// <summary> Constructor with reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input. Only data in the "active" area will be copied. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        /// <param name="order"> The permutation vector to apply to the dimensions when copying. Input dimension `i` will get copied to output dimension `order[i]`. If left empty, no reordering is done. 
        //    For instance, to reorder the normal interleaved image order into a planar order, the `order` parameter would be
        ///   set to {2, 0, 1} --- reordering {row, column, channel} to {channel, row, column} </param>
        /// <param name="paddingValue"> The value to use for output padding, if output shape is larger than input shape. </param>
        ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, const std::vector<int>& order, ValueType paddingValue = 0);


        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReorderDataNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        model::Shape ReorderInputToOutputLocation(model::Shape inputLocation) const;
        model::Shape ReorderOutputToInputLocation(model::Shape outputLocation) const;

        std::vector<llvm::Value*> ReorderInputToOutputLocation(std::vector<llvm::Value*> inputLocation) const;
        std::vector<llvm::Value*> ReorderOutputToInputLocation(std::vector<llvm::Value*> outputLocation) const;

        void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: inputShape, outputShape, paddingValue
    
    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;
        std::vector<int> _outputDimensionOrder;

        ValueType _paddingValue;
    };
}
}

#include "../tcc/ReorderDataNode.tcc"
