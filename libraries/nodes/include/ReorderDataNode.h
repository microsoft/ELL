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
    // TODO: unify DataShape with PortMemoryLayout
    class DataShape : public utilities::IArchivable
    {
    public:
        static constexpr size_t Dimension = 3;

        DataShape() = default;
        DataShape(const std::array<size_t, Dimension>& extent);
        DataShape(const std::array<size_t, Dimension>& extent, const std::array<size_t, Dimension>& padding);
        DataShape(const std::array<size_t, Dimension>& extent, const std::array<size_t, Dimension>& padding, const std::array<size_t, Dimension>& order);
        // DataShape(const std::array<size_t, Dimension>& extent, const std::array<size_t, Dimension>& padding, const std::array<size_t, Dimension>& stride);

        size_t NumEntries() const;
        size_t GetMemorySize() const;
        size_t GetExtent(int dimension) const { return _extent[dimension]; }
        size_t GetStride(int dimension) const { return _stride[dimension]; }
        size_t GetOffset(int dimension) const { return _offset[dimension]; }

        size_t GetEntryOffset(const std::array<int, Dimension>& location) const;
        bool IsOutOfBounds(const std::array<int, Dimension>& location) const;

        // Implemented in LLVM IR:
        llvm::Value* EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::array<llvm::Value*, Dimension>& location) const;
        llvm::Value* EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::array<llvm::Value*, Dimension>& location) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "DataShape"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void ComputeExtents(const std::array<size_t, Dimension>& order);
        size_t GetDataOffset() const; // offset for {0,0,0...}

        std::array<size_t, Dimension> _extent;
        std::array<size_t, Dimension> _stride;
        std::array<size_t, Dimension> _offset; // or do we just need total offset?
        size_t _totalSize = 0;
    };

    template <typename ValueType>
    class ReorderDataNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReorderDataNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="inputShape"> A description of the input memory shape. </param>
        /// <param name="outputShape"> A description of the desired output memory shape. </param>
        /// <param name="paddingValue"> The value to use for output padding, if output shape is larger than input shape. </param>
        ReorderDataNode(const model::PortElements<ValueType>& input, const DataShape& inputShape, const DataShape& outputShape, ValueType paddingValue = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReorderDataNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        DataShape _inputShape;
        DataShape _outputShape;
        ValueType _paddingValue;
    };
}
}

#include "../tcc/ReorderDataNode.tcc"
