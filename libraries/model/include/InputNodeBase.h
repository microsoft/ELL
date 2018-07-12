////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNodeBase.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "Port.h"

// utilities
#include "ArchiveVersion.h"
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    /// <summary> Base class for a node that represents an input to the system. </summary>
    class InputNodeBase : public CompilableNode
    {
    public:
        /// <summary> Gets the output port. </summary>
        ///
        /// <returns> The output port. </returns>
        const OutputPortBase& GetOutputPort() const { return _outputBase; }

        /// <summary> Returns the dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() { return _outputBase.Size(); }

        /// <summary> Gets the output type of this node </summary>
        ///
        /// <returns> The output type of this node </returns>
        Port::PortType GetOutputType() const { return _outputBase.GetType(); }

        /// <summary> Gets the output shape </summary>
        ///
        /// <returns> The output shape </returns>
        MemoryShape GetShape() const;

    protected:
        InputNodeBase(OutputPortBase& output, MemoryShape shape);

        // Constructor for derived classes that need to set the input port on CompilableNode.
        InputNodeBase(InputPortBase& input, OutputPortBase& output, MemoryShape shape);

        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return false; }
        void SetShape(const MemoryShape& shape);
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;

    private:
        OutputPortBase& _outputBase;
    };

    /// <summary> Base class for a node that represents a source to the system. </summary>
    class SourceNodeBase : public InputNodeBase
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
        // Note: Source nodes still receive timestamps as input, even though data is retrieved through callbacks.
        // Therefore, they have input ports.
        SourceNodeBase(InputPortBase& input, OutputPortBase& output, MemoryShape shape, const std::string& callbackName)
            : InputNodeBase(input, output, shape), _callbackName(callbackName)
        {
        }

    private:
        std::string _callbackName;
    };
}
}
