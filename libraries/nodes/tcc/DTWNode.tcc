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
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _sampleDimension(0), _prototypeLength(0), _threshold(0)
    {
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype, double confidenceThreshold)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName),  _output(this, outputPortName, 1), _prototype(prototype)
    {
        _sampleDimension = input.Size();
        _prototypeLength = prototype.size();
        _dPrev.resize(_prototypeLength + 1);
        _d.resize(_prototypeLength + 1);

        // TODO: compute threshold from confidenceThreshold and variance of sample
        double variance = 392.0529540761332; //this is the variance of the nextSlidePrototype
        _threshold = std::sqrt(-2 * std::log(confidenceThreshold)) * variance;
    }

    template <typename ValueType>
    DTWNode<ValueType>::DTWNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype, double threshold, UseRawThreshold)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName),  _output(this, outputPortName, 1), _prototype(prototype)
    {
        _sampleDimension = input.Size();
        _prototypeLength = prototype.size();
        _dPrev.resize(_prototypeLength + 1);
        _d.resize(_prototypeLength + 1);

        std::fill(_dPrev.begin()+1, _dPrev.end(), 99999.0);
        _dPrev[0] = 0.0;

        _threshold = threshold;
    }

    template <typename T>
    float euclideanDistanceFunction(const std::vector<T>& a, const std::vector<T>& b)
    {
        int s = 0;
        for (int index = 0; index < a.size(); index++)
        {
            s += (a[index] - b[index]) * (a[index] - b[index]);
        }
        return std::sqrt(s);
    }

    template <typename ValueType>
    void DTWNode<ValueType>::Compute() const
    {
        std::vector<ValueType> input = _input.GetValue();
        _d[0] = 0;
        for(size_t index = 1; index < _prototypeLength+1; ++index)
        {
            auto dist = euclideanDistanceFunction(_prototype[index-1], input);
            if(_d[index-1] <= _dPrev[index] && _d[index-1] < _dPrev[index-1])
            {
                _d[index] = dist + _d[index-1];
            }
            else if(_dPrev[index] <= _d[index-1] && _dPrev[index] <= _dPrev[index-1])
            {
                _d[index] = dist + _dPrev[index];
            }
            else
            {
                _d[index] = dist + _dPrev[index-1];
            }
        }

        auto result = _d[_prototypeLength] / _threshold;
        std::swap(_dPrev, _d);
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void DTWNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newinput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DTWNode<ValueType>>(newinput, _prototype, _threshold, UseRawThreshold());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DTWNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        // archiver["prototype"] << _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        archiver["threshold"] << _threshold;
    }

    template <typename ValueType>
    void DTWNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        // archiver["prototype"] >> _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        archiver["threshold"] >> _threshold;
    }
}
}
