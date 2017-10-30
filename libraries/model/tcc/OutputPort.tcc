////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    //
    // OutputPort
    //
    template <typename ValueType>
    OutputPort<ValueType>::OutputPort(const class Node* node, std::string name, size_t size)
        : OutputPortBase(node, name, OutputPortBase::GetPortType<ValueType>(), size)
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
        std::vector<double> result;
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
}
}
