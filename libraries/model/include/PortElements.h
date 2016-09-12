////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PortElements.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OutputPort.h"
#include "Port.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"

// stl
#include <algorithm>
#include <cassert>
#include <vector>

namespace emll
{
/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Represents a single value from an output port </summary>
    class PortElementBase
    {
    public:
        PortElementBase() = default;
        virtual ~PortElementBase() = default;

        /// <summary> Creates a PortElementBase representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElementBase(const OutputPortBase& port, size_t index);

        /// <summary> Returns the type of the value referenced </summary>
        ///
        /// <returns> The type of the value referenced </returns>
        Port::PortType GetType() const { return _referencedPort->GetType(); }

        /// <summary> The index within the port of the element this refers to </summary>
        ///
        /// <returns> The index of the element this refers to </returns>
        size_t GetIndex() const { return _index; }

        /// <summary> The port this element refers to </summary>
        ///
        /// <returns> The port this element refers to </returns>
        const OutputPortBase* ReferencedPort() const { return _referencedPort; }

        /// <summary> Equality operator. </summary>
        ///
        /// <returns> true if this element is equivalent to other. </returns>
        bool operator==(const PortElementBase& other) const;

    protected:
        const OutputPortBase* _referencedPort = nullptr;
        size_t _index = 0;
    };

    /// <summary> Represents a single value from an output port </summary>
    template <typename ValueType>
    class PortElement : public PortElementBase
    {
    public:
        PortElement() = default;

        /// <summary> Creates a PortElement representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElement(const OutputPortBase& port, size_t index)
            : PortElementBase(port, index) {}

        /// <summary> The port this element refers to </summary>
        ///
        /// <returns> The port this element refers to </returns>
        const OutputPort<ValueType>* ReferencedPort() const { return static_cast<const OutputPort<ValueType>*>(_referencedPort); }
    };

    /// <summary> Represents a contiguous set of values from an output port </summary>
    class PortRange : public utilities::IArchivable
    {
    public:
        PortRange() = default;

        /// <summary> Creates a PortRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        PortRange(const OutputPortBase& port);

        /// <summary> Creates a PortRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortRange(const OutputPortBase& port, size_t index);

        /// <summary> Creates a PortRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        PortRange(const OutputPortBase& port, size_t startIndex, size_t numValues);

        /// <summary> Returns the type of the values referenced </summary>
        ///
        /// <returns> The type of the values referenced </returns>
        Port::PortType GetType() const { return _referencedPort->GetType(); }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const;

        /// <summary> The index of the first element this range refers to </summary>
        ///
        /// <returns> The index of the first element this range refers to </returns>
        size_t GetStartIndex() const { return _startIndex; }

        /// <summary> The port this range refers to </summary>
        ///
        /// <returns> The port this range refers to </returns>
        const OutputPortBase* ReferencedPort() const { return _referencedPort; }

        /// <summary> Indicates if this range consists of the entire port's output </summary>
        ///
        /// <returns> true if this range spans the port's entire range </returns>
        bool IsFullPortRange() const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortRange"; }

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

        /// <summary> Equality operator. </summary>
        ///
        /// <returns> true if this range is equivalent to other. </returns>
        bool operator==(const PortRange& other) const;

        /// <summary> Checks if another range immediately follows this one </summary>
        ///
        /// <param name="other"> The other range </param>
        /// <returns> `true` if the other range's elements immediately follow this range's elements in the same port </returns>
        bool IsAdjacent(const PortRange& other) const;

        /// <summary> Adds the elements from another range to the end of this range, if they're contiguous </summary>
        ///
        /// <param name="other"> The other range </param>
        void Append(const PortRange& other);

    private:
        const OutputPortBase* _referencedPort = nullptr;
        size_t _startIndex = 0;
        size_t _numValues = 0;
        bool _isFixedSize = true;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class PortElementsBase : public utilities::IArchivable
    {
    public:
        PortElementsBase() = default;

        PortElementsBase(const OutputPortBase& port);
        PortElementsBase(const OutputPortBase& port, size_t startIndex);
        PortElementsBase(const OutputPortBase& port, size_t startIndex, size_t numValues);
        PortElementsBase(const PortRange& range);
        PortElementsBase(const std::vector<PortRange>& ranges);

        virtual ~PortElementsBase() = default;

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> The number of ranges in this list </summary>
        ///
        /// <returns> The number of ranges in this list </returns>
        size_t NumRanges() const { return _ranges.size(); }

        /// <summary> Gets a vector of range objects </summary>
        ///
        /// <returns> A std::vector of PortRange objects </returns>
        const std::vector<PortRange>& GetRanges() const { return _ranges; }

        /// <summary> Reserves space for the given number of ranges </summary>
        ///
        /// <param name="numRanges"> The number of ranges to reserve space for </param>
        void Reserve(size_t numRanges);

        /// <summary> Gets an element in the elements. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// <returns> The specified element. </returns>
        PortElementBase GetElement(size_t index) const;

        /// <summary> Appends a set of elements to this set of elements. </summary>
        ///
        /// <param name="other"> The PortElements to append to this one. </param>
        void Append(const PortElementsBase& other);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortElementsBase"; }

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

        /// <summary> Consolidates adjacent ranges </summary>
        virtual void Consolidate();

    protected:
        void ComputeSize();
        void AddRange(const PortRange& range);

    private:
        std::vector<PortRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class PortElements : public PortElementsBase
    {
    public:
        PortElements() = default;

        /// <summary> Creates a PortElements representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        PortElements(const OutputPort<ValueType>& port);

        /// <summary> Creates a PortElements representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElements(const OutputPort<ValueType>& port, size_t index);

        /// <summary> Creates a PortElements representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        PortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a PortElements representing a single element from a given port </summary>
        ///
        /// <param name="element"> The element </param>
        PortElements(const PortElement<ValueType>& element);

        /// <summary> Creates a PortElements by concatenating a set of PortElement objects together </summary>
        ///
        /// <param name="elements"> The vector of elements to concantenate together </param>
        PortElements(const std::vector<PortElement<ValueType>>& element);

        /// <summary> Creates a PortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        PortElements(const std::initializer_list<PortElements<ValueType>>& groups);

        /// <summary> Creates a PortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The vector of groups to concantenate together </param>
        PortElements(const std::vector<PortElements<ValueType>>& groups);

        /// <summary> Creates a PortElements representing a single value from a given PortElements </summary>
        ///
        /// <param name="elements"> The PortElements to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElements(const PortElements<ValueType>& elements, size_t index);

        /// <summary> Creates a PortElements representing a range of values from a given PortElements </summary>
        ///
        /// <param name="elements"> The PortElements to take a value from </param>
        /// <param name="startIndex"> The index of the first value to use </param>
        /// <param name="numValues"> The number of values to take </param>
        PortElements(const PortElements<ValueType>& elements, size_t startIndex, size_t numValues);

        /// <summary> Creates a typed `PortElements` from an untyped `PortElementsBase` </summary>
        ///
        /// <param name="elements"> The PortElements to take a value from </param>
        /// <param name="index"> The index of the value </param>
        /// <exception cref="utilities::InputException"> Throws an `InputException` with code `typeMismatch` if
        ///   any of the input elements have the wrong type </exception>
        explicit PortElements(const PortElementsBase& other);

        /// <summary> Gets an element in the elements. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// <returns> The specified element. </returns>
        PortElement<ValueType> GetElement(size_t index) const;

        /// <summary> The port this element refers to </summary>
        ///
        /// <returns> The port this element refers to </returns>
        const OutputPort<ValueType>* ReferencedPort() const { return static_cast<const OutputPort<ValueType>*>(this->PortElementBase::ReferencedPort()); }
 
        /// <summary> Appends a set of elements to this set of elements. </summary>
        ///
        /// <param name="other"> The PortElements to append to this one. </param>
        void Append(const PortElements<ValueType>& other);
 
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("PortElements"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        using type = ValueType;
    };

    //
    // Helper functions
    //

    /// <summary> Creates a PortElements representing all the values from a given port </summary>
    ///
    /// <param name="port"> The port to take values from </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port);

    /// <summary> Creates a PortElements representing a single value from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="index"> The index of the value </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t index);

    /// <summary> Creates a PortElements representing a range of values from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="startIndex"> The index of the first value to take </param>
    /// <param name="numValues"> The number of values to take </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

    /// <summary> Creates a PortElements by concatenating together one or more PortElementss
    ///
    /// <param name="ref"> The PortElementss to concatenate together </param>
    /// <returns> The composite PortElements </returns>
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs);
}
}

// custom specialization of std::hash so we can keep PortRanges in containers that require hashable types
namespace std
{
/// <summary> Implements a hash function for the PortRange class, so that it can be used with associative containers (maps, sets, and the like). </summary>
template <>
class hash<emll::model::PortElementBase>
{
public:
    typedef emll::model::PortElementBase argument_type;
    typedef std::size_t result_type;

    /// <summary> Computes a hash of the input value. </summary>
    ///
    /// <returns> A hash value for the given input. </returns>
    result_type operator()(argument_type const& id) const;
};

template <>
class hash<emll::model::PortRange>
{
public:
    typedef emll::model::PortRange argument_type;
    typedef std::size_t result_type;

    /// <summary> Computes a hash of the input value. </summary>
    ///
    /// <returns> A hash value for the given input. </returns>
    result_type operator()(argument_type const& id) const;
};
}

#include "../tcc/PortElements.tcc"
