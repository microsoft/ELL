////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SliceNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class IRMapCompiler;
    class ModelTransformer;

    /// <summary> A node that returns a subset of the entries from an output port. </summary>
    template <typename ValueType>
    class SliceNode : public CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const InputPort<ValueType>& input = _input;
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SliceNode();

        /// <summary> Constructor </summary>
        /// <param name="port"> The port to take input values from. </param>
        /// <param name="start"> The start index for the first (largest) physical dimension of the active area. </param>
        /// <param name="count"> The size of the first (largest) physical dimension of the output to return. </param>
        SliceNode(const OutputPortBase& port, int start, int count);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SliceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(ModelTransformer& transformer) const override;

        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
        int _largestDimensionStart = 0;
        int _largestDimensionCount = 0;
    };
} // namespace model
} // namespace ell

#include "../tcc/SliceNode.tcc"
