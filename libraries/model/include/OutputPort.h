////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"

// utilities
#include "IArchivable.h"

// stl
#include <vector>
#include <memory>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Base class for output ports </summary>
    class OutputPortBase : public Port
    {
    public:
        OutputPortBase() = default;
        OutputPortBase(const OutputPortBase& other) = delete;

        /// <summary> Notify this port that it is being referenced </summary>
        void ReferencePort() const { _isReferenced = true; }

        /// <summary> Returns the dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        virtual size_t Size() const override { return _size; }

        /// <summary> Indicate if this port is referenced. </summary>
        ///
        /// <returns> Returns true if the port is referenced by another node. </returns>
        bool IsReferenced() { return _isReferenced; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "OutputPortBase"; }

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

    protected:
        OutputPortBase(const class Node* node, std::string name, PortType type, size_t size);

        size_t _size = 0;
        mutable bool _isReferenced;
    };

    /// <summary> Represents an output from a node </summary>
    template <typename ValueType>
    class OutputPort : public OutputPortBase
    {
    public:
        OutputPort() = default;
        OutputPort(const OutputPort&) = default; 

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="name"> The name of this port </param>
        /// <param name="size"> The dimensionality of this port </param>
        OutputPort(const class Node* node, std::string name, size_t size);

        /// <summary> Returns the cached output from this port </summary>
        ///
        /// <returns> The cached output from this port </returns>
        const std::vector<ValueType>& GetOutput() const { return _cachedOutput; }

        /// <summary> Returns one element of the cached output from this port </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The cached output for the element </returns>
        ValueType GetOutput(size_t index) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        void SetOutput(std::vector<ValueType> values) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputPort"); }

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

    private:
        mutable std::vector<ValueType> _cachedOutput;
    };
}

#include "../tcc/OutputPort.tcc"