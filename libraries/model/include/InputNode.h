////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputNodeBase.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "PortMemoryLayout.h"

// utilities
#include "IArchivable.h"
#include "TypeName.h"

#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class ModelTransformer;

    /// <summary> A node that represents an input to the system. </summary>
    template <typename ValueType>
    class InputNode : public InputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Constructor </summary>
        InputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="size"> The input size </param>
        InputNode(size_t size);

        /// <summary> Constructor </summary>
        ///
        /// <param name="shape"> The input node's output shape </param>
        InputNode(MemoryShape shape);

        /// <summary> Constructor </summary>
        ///
        /// <param name="layout"> The input node's output memory layout </param>
        InputNode(const PortMemoryLayout& layout);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The value for this node to output </param>
        void SetInput(ValueType inputValue);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The values for this node to output </param>
        void SetInput(std::vector<ValueType> inputValues);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("InputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary></summary>
        size_t Size() const { return _output.Size(); }

    protected:
        void Compute() const override;
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(ModelTransformer& transformer) const override;

        std::vector<ValueType> _inputValues;
        OutputPort<ValueType> _output;
    };
} // namespace model
} // namespace ell

#include "../tcc/InputNode.tcc"
