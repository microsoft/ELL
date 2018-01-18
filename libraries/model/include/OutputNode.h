////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelTransformer.h"
#include "OutputNodeBase.h"
#include "OutputPort.h"

// utilities
#include "IArchivable.h"

// stl
#include <memory>
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{

    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public OutputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* shapeName = "shape";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The `PortElements<>` to get the input data from </param>
        OutputNode(const model::PortElements<ValueType>& input);

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The `PortElements<>` to get the input data from </param>
        /// <param name="shape"> The shape of the input data </param>
        OutputNode(const model::PortElements<ValueType>& input, const OutputShape& shape);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(ModelTransformer& transformer) const override;

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        void Compute() const override;
        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
    };
}
}

#include "../tcc/OutputNode.tcc"
