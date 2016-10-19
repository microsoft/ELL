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
        : Node({ &_input, &_sample }, { &_output }), _input(this, {}, inputPortName), _sample(this, {}, samplePortName), _output(this, outputPortName, 1), _sampleDimension(0), _sampleLength(0)
    {
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const model::PortElements<ValueType>& sample, double confidenceThreshold)
        : Node({ &_input, &_sample }, { &_output }), _input(this, input, inputPortName), _sample(this, sample, samplePortName), _output(this, outputPortName, 1)
    {
        _sampleDimension = input.Size();
        _sampleLength = sample.Size() / input.Size();
        _dPrev.resize(_sampleLength + 1);
        _d.resize(_sampleLength + 1);

        _threshold = threshold;
        // TODO: compute threshold from confidenceThreshold and variance of sample
        // float confidenceThreshold = 0.2; //this can be within the range (0,1)
        // float variance = 392.0529540761332; //this is the variance of the nextSlidePrototype
        // float _threshold = sqrt(-2 * log(confidenceThreshold)) * variance;
    }

    template <typename T>
    float euclideanDistanceFunction(const std::vector<T>& a, const std::vector<T>& b)
    {
        int s = 0;
        for (int i = 0; i < a.size(); i++)
        {
            s += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return std::sqrt(s);
    }

    template <typename ValueType>
    void DTWNode<ValueType>::Compute() const
    {
        std::vector<ValueType> input = _input.GetValue();
        _d[0] = 0;
        for(size_t index = 1; index < _sampleLength+1; ++index)
        {
            auto dist = euclideanDistanceFunction(GetSample(index-1), input);
            if(_d[i-1] <= _dPrev[i] && _d[i-1] < _dPrev[i-1])
            {
                _d[i] = dist + _d[i-1]
            }
            else if(_dPrev[i] <= _d[i-1] && _dPrev[i] <= _dPrev[i-1])
            {
                _d[i] = dist + _dPrev[i];
            }
            else
            {
                _d[i] = dist + _dPrev[i-1];
            }
        }

        auto result = _d[_sampleLength] / _threshold;
        std::swap(_dPrev, _d);
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
