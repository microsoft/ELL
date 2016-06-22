////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputRef.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputRef
    //

    template <typename ValueType>
    OutputRef<ValueType>::OutputRef(const OutputPort<ValueType>& port) : UntypedOutputRef(port)
    {
    }

    template <typename ValueType>
    OutputRef<ValueType>::OutputRef(const OutputPort<ValueType>& port, size_t startIndex) : UntypedOutputRef(OutputRange(port, startIndex))
    {
    }

    template <typename ValueType>
    OutputRef<ValueType>::OutputRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : UntypedOutputRef(OutputRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    OutputRef<ValueType>::OutputRef(const std::initializer_list<OutputRef<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group._ranges)
            {
                _ranges.push_back(range);
            }
        }
        ComputeSize();
    }

    template <typename ValueType>
    OutputRef<ValueType>::OutputRef(const std::vector<OutputRef<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group._ranges)
            {
                _ranges.push_back(range);
            }
        }
        ComputeSize();
    }
}
