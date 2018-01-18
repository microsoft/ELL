////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BufferNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "IArchivable.h"
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that buffers the input and allows access to the buffer. </summary>
    template <typename ValueType>
    class BufferNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        BufferNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to buffer. </param>
        /// <param name="windowSize"> The number of samples to accumulate in the buffer. </param>
        BufferNode(const model::PortElements<ValueType>& input, size_t windowSize);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BufferNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Return the window size </summary>
        ///
        /// <returns> The window size </returns>
        size_t GetWindowSize() const { return _windowSize; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; } // stored state: windowSize
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<ValueType> _samples;
        size_t _windowSize;
    };
}
}

#include "../tcc/BufferNode.tcc"