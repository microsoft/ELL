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

// math
#include "Tensor.h"

// utilities
#include "IArchivable.h"

// stl
#include <string>

namespace ell
{
namespace model
{
    class IRMapCompiler;

    using OutputShape = ell::math::TensorShape;

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
        /// <returns> The output shpe. </returns>
        OutputShape GetShape() const { return _shape; }

    protected:
        OutputNodeBase(InputPortBase& input, OutputPortBase& output, const ell::math::TensorShape& shape);
        OutputNodeBase(const std::vector<InputPortBase*>& inputs, OutputPortBase& output, const ell::math::TensorShape& shape);
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return false; }
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        ell::utilities::ArchiveVersion GetArchiveVersion() const override;

        void SetShape(const OutputShape& shape) { _shape = shape; } // STYLE discrepancy
    private:
        InputPortBase& _inputBase;
        OutputPortBase& _outputBase;
        OutputShape _shape;
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
        SinkNodeBase(InputPortBase& input, InputPortBase& trigger, OutputPortBase& output, ell::math::TensorShape shape, const std::string& callbackName)
            : OutputNodeBase({ &input, &trigger }, output, shape), _callbackName(callbackName)
        {
        }

    private:
        std::string _callbackName;
    };
}
}

