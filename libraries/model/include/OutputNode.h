////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// utilities
#include "IArchivable.h"

// stl
#include <memory>
#include <string>
#include <vector>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public Node
    {
    public:
        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        OutputNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/OutputNode.tcc"
