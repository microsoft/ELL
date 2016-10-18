////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DTWNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace nodes
{
    template <typename ValueType>
    DTWNode<ValueType>::DTWNode()
        : Node({ &_input, &_sample }, { &_output }), _input(this, {}, inputPortName), _sample(this, {}, samplePortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const model::PortElements<ValueType>& sample)
        : Node({ &_input, &_sample }, { &_output }), _input(this, input, inputPortName), _sample(this, sample, samplePortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    void DTWNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            result += _input[index] * _sample[index];
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void DTWNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newinput = transformer.TransformPortElements(_input.GetPortElements());
        auto newsample = transformer.TransformPortElements(_sample.GetPortElements());
        auto newNode = transformer.AddNode<DTWNode<ValueType>>(newinput, newsample);
        transformer.MapNodeOutput(output, newNode->output);
    }

    // template <typename ValueType>
    // bool DTWNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    // {
    //     // Maybe... in reality, dot product will likely want to be computed as in Compute() above
    //     auto newinput = transformer.TransformPortElements(_input.GetPortElements());
    //     auto newsample = transformer.TransformPortElements(_sample.GetPortElements());
    //     auto multNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newinput, newsample, BinaryOperationType::coordinatewiseMultiply);
    //     auto sumNode = transformer.AddNode<SumNode<ValueType>>(multNode->output);

    //     transformer.MapNodeOutput(output, sumNode->output);
    //     return true;
    // }

    template <typename ValueType>
    void DTWNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[samplePortName] << _sample;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType>
    void DTWNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[samplePortName] >> _sample;
        archiver[outputPortName] >> _output;
    }
}
}
