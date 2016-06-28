////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputPort
    //
    template <typename ValueType>
    void OutputPort<ValueType>::SetOutput(std::vector<ValueType> values) const
    {
        _cachedOutput = values;
    }
}