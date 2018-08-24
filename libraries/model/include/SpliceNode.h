////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SpliceNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "InputPort.h"
#include "Node.h"
#include "OutputPort.h"

// utilities
#include "Exception.h"
#include "TypeName.h"

// stl
#include <memory>
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class IRMapCompiler;
    class ModelTransformer;

    /// <summary> A node that concatenates the values from a number of output ports. </summary>
    template <typename ValueType>
    class SpliceNode : public CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SpliceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputPorts"> The output ports to concatenate. </param>
        SpliceNode(const std::vector<const OutputPortBase*>& inputPorts);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SpliceNode"); }

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
        static PortMemoryLayout ComputeOutputLayout(const std::vector<const OutputPortBase*>& inputPorts);

        std::vector<std::unique_ptr<InputPort<ValueType>>> _inputPorts;
        OutputPort<ValueType> _output;
    };
}
}

#include "../tcc/SpliceNode.tcc"
