////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "PortMemoryLayout.h"

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/IArchivable.h>

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

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node to which this port belongs. </param>
        /// <param name="name"> The name of this port. </param>
        /// <param name="type"> The datatype for this port. </param>
        /// <param name="layout"> The memory layout of the port's output. </param>
        OutputPortBase(const Node* node, std::string name, PortType type, const PortMemoryLayout& layout);

        ~OutputPortBase() override = default;

        /// <summary> Notify this port that it is being referenced </summary>
        void ReferencePort() const { _isReferenced = true; }

        /// <summary> Returns the size of the output </summary>
        ///
        /// <returns> The size of the output </returns>
        size_t Size() const override { return _layout.GetMemorySize(); }

        /// <summary> Sets the size of the output </summary>
        ///
        /// <param name="size> The size of the output </param>
        void SetSize(size_t size);

        /// <summary> Sets the memory layout of the output </summary>
        ///
        /// <param name="layout"> The memory layout of the output </param>
        void SetMemoryLayout(const PortMemoryLayout& layout);

        /// <summary> Returns the memory layout of the output </summary>
        ///
        /// <returns> The memory layout of the output </returns>
        PortMemoryLayout GetMemoryLayout() const override { return _layout; }

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
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        PortMemoryLayout _layout;
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
        OutputPort(const Node* node, std::string name, size_t size);

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="name"> The name of this port </param>
        /// <param name="layout"> The memory layout of the port's output. </param>
        OutputPort(const Node* node, std::string name, const PortMemoryLayout& layout);

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
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    //
    // OutputPort
    //
    template <typename ValueType>
    OutputPort<ValueType>::OutputPort(const Node* node, std::string name, size_t size) :
        OutputPortBase(node, name, OutputPortBase::GetPortType<ValueType>(), size)
    {
    }

    template <typename ValueType>
    OutputPort<ValueType>::OutputPort(const Node* node, std::string name, const PortMemoryLayout& layout) :
        OutputPortBase(node, name, OutputPortBase::GetPortType<ValueType>(), layout)
    {
    }

    template <typename ValueType>
    ValueType OutputPort<ValueType>::GetOutput(size_t index) const
    {
        return _cachedOutput[index];
    }

    template <typename ValueType>
    std::vector<double> OutputPort<ValueType>::GetDoubleOutput() const
    {
        std::vector<double> result(_cachedOutput.size());
        std::copy(_cachedOutput.begin(), _cachedOutput.end(), result.begin());
        return result;
    }

    template <typename ValueType>
    double OutputPort<ValueType>::GetDoubleOutput(size_t index) const
    {
        return static_cast<double>(_cachedOutput[index]);
    }

    template <typename ValueType>
    template <typename U>
    void OutputPort<ValueType>::SetOutput(std::initializer_list<U>&& values) const
    {
        this->SetOutput(std::begin(values), std::end(values));
    }

    template <typename ValueType>
    template <typename C>
    void OutputPort<ValueType>::SetOutput(C&& values) const
    {
        this->SetOutput(std::begin(values), std::end(values));
    }

    template <typename ValueType>
    template <typename It>
    void OutputPort<ValueType>::SetOutput(It begin, It end) const
    {
        _cachedOutput.assign(begin, end);
    }

    template <typename ValueType>
    void OutputPort<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        OutputPortBase::WriteToArchive(archiver);
    }

    template <typename ValueType>
    void OutputPort<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        OutputPortBase::ReadFromArchive(archiver);
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
