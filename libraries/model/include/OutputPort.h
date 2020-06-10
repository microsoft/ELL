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
#include <variant>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    class Node;
    class InputPortBase;

    /// <summary> Base class for output ports </summary>
    class OutputPortBase : public Port
    {
    public:
        OutputPortBase() = default;
        OutputPortBase(const OutputPortBase& other) = delete;

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

        ~OutputPortBase() override;

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
        bool IsReferenced() const;

        /// <summary> Get the ports that are referencing this port. </summary>
        ///
        /// <returns> The ports that are referencing this port. </returns>
        const std::vector<const InputPortBase*>& GetReferences() const;

        /// <summary> Returns the cached output from this port </summary>
        ///
        /// <returns> The cached output from this port </returns>
        template <typename ValueType>
        const std::vector<ValueType>& GetOutput() const;

        /// <summary> Returns one element of the cached output from this port </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The cached output for the element </returns>
        template <typename ValueType>
        ValueType GetOutput(size_t index) const;

        /// <summary> Gets the output of this port, converted to `double`. </summary>
        ///
        /// <returns> The output of this port, converted to `double`. </returns>
        std::vector<double> GetDoubleOutput() const;

        /// <summary> Gets the output of an element, converted to a `double`. </summary>
        ///
        /// <param name="index"> The index of the element to return. </param>
        ///
        /// <returns> The output element, converted to a `double`. </returns>
        double GetDoubleOutput(size_t index) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        /// <typeparam name="ValueType"> The fundamental type used by values </typeparam>
        template <typename ValueType>
        void SetOutput(std::initializer_list<ValueType>&& values) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        /// <typeparam name="ContainerType"> The container type that holds the values </typeparam>
        template <typename ContainerType>
        void SetOutput(ContainerType&& values) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=begin> The iterator representing the start value </param>
        /// <param name=end> The iterator representing the end value  </param>
        /// <typeparam name="IteratorType"> The iterator type </typeparam>
        template <typename IteratorType>
        void SetOutput(IteratorType begin, IteratorType end) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "OutputPortBase"; }

    protected:
        friend class InputPortBase;

        void AddReference(const InputPortBase* reference) const;
        void RemoveReference(const InputPortBase* reference) const;
        bool HasReference(const InputPortBase* reference) const;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        PortMemoryLayout _layout;
        mutable std::vector<const InputPortBase*> _references;

    private:
        void InitializeCachedOutput();

        mutable std::variant<std::vector<double>,
                             std::vector<float>,
                             std::vector<int64_t>,
                             std::vector<int32_t>,
                             std::vector<bool>>
            _cachedOutput;
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
        const std::vector<ValueType>& GetOutput() const { return OutputPortBase::GetOutput<ValueType>(); }

        /// <summary> Returns one element of the cached output from this port </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The cached output for the element </returns>
        ValueType GetOutput(size_t index) const;

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
    };
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    //
    // OutputPortBase
    //
    template <typename ValueType>
    const std::vector<ValueType>& OutputPortBase::GetOutput() const
    {
        return std::get<std::vector<ValueType>>(_cachedOutput);
    }

    template <typename ValueType>
    ValueType OutputPortBase::GetOutput(size_t index) const
    {
        return GetOutput<ValueType>()[index];
    }

    template <typename ValueType>
    void OutputPortBase::SetOutput(std::initializer_list<ValueType>&& values) const
    {
        SetOutput(std::begin(values), std::end(values));
    }

    template <typename ContainerType>
    void OutputPortBase::SetOutput(ContainerType&& values) const
    {
        SetOutput(std::begin(values), std::end(values));
    }

    template <typename IteratorType>
    void OutputPortBase::SetOutput(IteratorType begin, IteratorType end) const
    {
        using ValueType = typename std::iterator_traits<IteratorType>::value_type;
        using VectorType = std::vector<ValueType>;
        std::get<VectorType>(_cachedOutput).assign(begin, end);
    }

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
        return OutputPortBase::GetOutput<ValueType>(index);
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
