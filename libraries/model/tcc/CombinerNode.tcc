////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CombinerNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{

    template <typename ValueType>
    CombinerNode<ValueType>::CombinerNode(const std::vector<OutputRange<ValueType>>& inputs) : Node({}, { &_output }), _output(this, 0, 0)
    {
        size_t totalSize = 0;
        size_t index = 0;
        for(auto& input: inputs)
        {
            _inputRanges.emplace_back(this, index, &(input.port), input.startIndex, input.numValues);
            totalSize += input.numValues;
            index += 1;
        }

        for (auto& range : _inputRanges)
        {
            AddInputPort(&range.port);
        }

        _output.SetSize(totalSize);
    };

    template <typename ValueType>
    CombinerNode<ValueType>::CombinerNode(const std::initializer_list<OutputRange<ValueType>>& inputs) : Node({}, { &_output }), _output(this, 0, 0)
    {
        size_t totalSize = 0;
        size_t index = 0;
        for (auto& input : inputs)
        {
            _inputRanges.emplace_back(this, index, &(input.port), input.startIndex, input.numValues);
            totalSize += input.numValues;
            index += 1;
        }

        for (auto& range : _inputRanges)
        {
            AddInputPort(&range.port);
        }

        _output.SetSize(totalSize);
    };

    template <typename ValueType>
    void CombinerNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        for(const auto& inputRange: _inputRanges)
        {
            auto inVec = inputRange.port.GetValue<ValueType>();
            output.insert(output.end(), inVec.begin()+inputRange.startIndex, inVec.begin()+inputRange.startIndex+inputRange.numValues);
        }

        _output.SetOutput(output);
    };
}
