////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNodeBase.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortMemoryLayout.h"

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/IArchivable.h>

#include <string>

namespace ell
{
namespace model
{
    using MemoryShape = utilities::MemoryShape;

    class IRMapCompiler;

    /// <summary> A node that represents an output from the system. </summary>
    class OutputNodeBase : public CompilableNode
    {
    public:
        using Node::GetInputPort;
        using Node::GetOutputPort;

        /// <summary> Gets the input port. </summary>
        ///
        /// <returns> The input port. </returns>
        const InputPortBase& GetInputPort() const { return _inputBase; }

        /// <summary> Gets the output port. </summary>
        ///
        /// <returns> The output port. </returns>
        const OutputPortBase& GetOutputPort() const { return _outputBase; }

        /// <summary> Gets the output shape. </summary>
        ///
        /// <returns> The output shape. </returns>
        MemoryShape GetShape() const;

        /// <summary> Attempts to set the memory layout order of all the output ports </summary>
        ///
        /// <param name="order"> The memory layout order to be applied to all the output ports </summary>
        /// <returns> If the node supports the output memory layout order, true, else false </returns>
        bool TrySetOutputLayout(const utilities::DimensionOrder& order) override
        {
            return _outputBase.GetMemoryLayout().GetLogicalDimensionOrder() == order;
        }

    protected:
        OutputNodeBase(InputPortBase& input, OutputPortBase& output, const MemoryShape& shape);
        OutputNodeBase(const std::vector<InputPortBase*>& inputs, OutputPortBase& output, const MemoryShape& shape);
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return false; }
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        ell::utilities::ArchiveVersion GetArchiveVersion() const override;

        void SetShape(const MemoryShape& shape);

    private:
        InputPortBase& _inputBase;
        OutputPortBase& _outputBase;
    };

    /// <summary> Base class for a node that represents a sink from the system. </summary>
    class SinkNodeBase : public OutputNodeBase
    {
    public:
        /// <summary> Gets the callback function name for this node. </summary>
        ///
        /// <returns> The callback name. </returns>
        std::string GetCallbackName() const { return _callbackName; };

        /// <summary> Sets the callback function name for this node. </summary>
        ///
        /// <param name="name"> The callback name to set. </param>
        void SetCallbackName(const std::string& name) { _callbackName = name; };

    protected:
        SinkNodeBase(InputPortBase& input, InputPortBase& trigger, OutputPortBase& output, const MemoryShape& shape, const std::string& callbackName) :
            OutputNodeBase({ &input, &trigger }, output, shape),
            _callbackName(callbackName)
        {
        }

    private:
        std::string _callbackName;
    };
} // namespace model
} // namespace ell
