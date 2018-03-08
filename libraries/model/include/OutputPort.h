////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"

// utilities
#include "IArchivable.h"

// stl
#include <memory>
#include <vector>

namespace ell
{
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
        OutputPortBase(OutputPortBase&& other) = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node to which this port belongs. </param>
        /// <param name="name"> The name of this port. </param>
        /// <param name="type"> The datatype for this port. </param>
        /// <param name="size"> The size of the port's output. </param>
        OutputPortBase(const Node* node, std::string name, PortType type, size_t size);

        ~OutputPortBase() override = default;

        /// <summary> Notify this port that it is being referenced </summary>
        void ReferencePort() const { _isReferenced = true; }

        /// <summary> Returns the size of the output </summary>
        ///
        /// <returns> The size of the output </returns>
        size_t Size() const override { return _size; }

        /// <summary> Sets the size of the output </summary>
        ///
        /// <param name="size> The size of the output </param>
        void SetSize(size_t size);

        /// <summary> Indicate if this port is referenced. </summary>
        ///
        /// <returns> Returns true if the port is referenced by another node. </returns>
        bool IsReferenced() const { return _isReferenced; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "OutputPortBase"; }

        /// <summary> Gets the output of this port, converted to `double`. </summary>
        ///
        /// <returns> The output of this port, converted to `double`. </returns>
        virtual std::vector<double> GetDoubleOutput() const { return {}; };

        /// <summary> Gets the output of an element, converted to a `double`. </summary>
        ///
        /// <param name="index"> The index of the element to return. </param>
        ///
        /// <returns> The output element, converted to a `double`. </returns>
        virtual double GetDoubleOutput(size_t index) const { return 0.0; };

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        size_t _size = 0;
        mutable bool _isReferenced = false;
    };

    /// <summary> Represents an output from a node </summary>
    template <typename ValueType>
    class OutputPort : public OutputPortBase
    {
    public:
        OutputPort() = default;
        OutputPort(const OutputPort&) = delete;

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="name"> The name of this port </param>
        /// <param name="size"> The size of this port </param>
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

        /// <summary> Gets the output of this port, converted to `double`. </summary>
        ///
        /// <returns> The output of this port, converted to `double`. </returns>
        std::vector<double> GetDoubleOutput() const override;

        /// <summary> Gets the output of an element, converted to a `double`. </summary>
        ///
        /// <param name="index"> The index of the element to return. </param>
        ///
        /// <returns> The output element, converted to a `double`. </returns>
        double GetDoubleOutput(size_t index) const override;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        /// <typeparam name="U"> The fundamental type used by values </typeparam>
        template <typename U>
        void SetOutput(std::initializer_list<U>&& values) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        /// <typeparam name="C"> The container type that holds the values </typeparam>
        template <typename C>
        void SetOutput(C&& values) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=begin> The iterator representing the start value </param>
        /// <param name=end> The iterator representing the end value  </param>
        /// <typeparam name="U"> The iterator type </typeparam>
        template <typename It>
        void SetOutput(It begin, It end) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputPort"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        mutable std::vector<ValueType> _cachedOutput;
    };
}
}

#include "../tcc/OutputPort.tcc"