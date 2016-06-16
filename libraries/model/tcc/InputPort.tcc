////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    InputPort::InputPort(const class Node* owningNode, size_t index, const OutputPort<ValueType>* output) : Port(owningNode, index, Port::GetTypeCode<ValueType>(), output->Size()), _referencedPort(output)
    {
        if (Port::GetTypeCode<ValueType>() != output->Type())
        {
            throw std::runtime_error("InputPort type doesn't match output");
        }
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort::GetValue() const
    {
        assert(ReferencedPort()->Node() != nullptr);

        // retrieve cached value from our connected output
        auto result = ReferencedPort()->Node()->GetOutputValue<ValueType>(ReferencedPort()->Index());
        assert(result.size() == Size());
        return result;
    }
}
