////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FullyConnectedLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    FullyConnectedLayer::FullyConnectedLayer(size_t numNodes, size_t numInputsPerNode, const std::vector<double>& weights) :
        _numInputs(numNodes * numInputsPerNode),
        _weights(numNodes, numInputsPerNode, weights),
        _output(numNodes)
    {
        if (weights.size() != (numNodes * numInputsPerNode))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "weights dimension for a fully connected layer should be the same as number of nodes times inputs per node");
        }
    }

    FullyConnectedLayer::FullyConnectedLayer(size_t numNodes, size_t numInputsPerNode, std::vector<double>&& weights) :
        _numInputs(numNodes * numInputsPerNode),
        _weights(numNodes, numInputsPerNode, std::move(weights)),
        _output(numNodes)
    {
        if ((_weights.NumRows() * _weights.NumColumns()) != (numNodes * numInputsPerNode))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "weights dimension for a fully connected layer should be the same as number of nodes times inputs per node");
        }
    }

    ILayer::LayerVector& FullyConnectedLayer::FeedForward(const ILayer::LayerVector& input)
    {
        
        math::Operations::Multiply(1.0, _weights, input, 0.0, _output);
        return _output;
    }

    void FullyConnectedLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["numInputs"] << _numInputs;

        archiver["weights_rows"] << _weights.NumRows();
        archiver["weights_columns"] << _weights.NumColumns();
        std::vector<double> temp;
        temp.assign(_weights.GetDataPointer(), _weights.GetDataPointer() + (size_t)(_weights.NumRows() * _weights.NumColumns()));
        archiver["weights_data"] << temp;

        temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void FullyConnectedLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["numInputs"] >> _numInputs;

        size_t rows = 0;
        size_t columns = 0;
        archiver["weights_rows"] >> rows;
        archiver["weights_columns"] >> columns;
        std::vector<double> temp;
        archiver["weights_data"] >> temp;
        _weights = ILayer::LayerMatrix(rows, columns, temp);

        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

