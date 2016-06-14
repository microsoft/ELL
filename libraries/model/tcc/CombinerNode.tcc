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
    CombinerNode<ValueType>::CombinerNode(const std::vector<InputRange>& inputs) : Node({}, { &_output }), _inputs(inputs), _output(this, 0, 0)
    {
        size_t totalSize = 0;
        for(const auto& input: inputs)
        {
            totalSize += input.numValues;
        }
        _output.SetSize(totalSize);
    };

    template <typename ValueType>
    void CombinerNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        for(const auto& inputRange: _inputs)
        {
            auto inVec = inputRange.port->GetOutput();
            output.insert(output.end(), inVec.begin()+inputRange.startIndex, inVec.begin()+inputRange.startIndex+inputRange.numValues);
        }

        _output.SetOutput(output);
    };
}
