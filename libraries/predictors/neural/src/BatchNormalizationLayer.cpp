////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BatchNormalizationLayer.h"

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{

    BatchNormalizationLayer::BatchNormalizationLayer(size_t numNodes, const std::vector<double>& mean, const std::vector<double>& variance, size_t applyCount) :
        _applyCount(applyCount),
        _mean(mean),
        _variance(variance),
        _output(numNodes)
    {
        if ((_mean.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "'mean' dimension times apply count should be the same as number of nodes");
        }
        if ((_variance.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "'variance' dimension times apply count should be the same as number of nodes");
        }
    }

    BatchNormalizationLayer::BatchNormalizationLayer(size_t numNodes, std::vector<double>&& mean, std::vector<double>&& variance, size_t applyCount) :
        _applyCount(applyCount),
        _mean(std::move(mean)),
        _variance(std::move(variance)),
        _output(numNodes)
    {
        if ((_mean.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "'mean' dimension times apply count should be the same as number of nodes");
        }
        if ((_variance.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "'variance' dimension times apply count should be the same as number of nodes");
        }
    }

    ILayer::LayerVector& BatchNormalizationLayer::FeedForward(const ILayer::LayerVector& input)
    {
        size_t inOutIndex = 0;
        size_t valueIndex = 0;
        while (inOutIndex < input.Size() && inOutIndex < _output.Size())
        {
            for (size_t ac = 0; ac < _applyCount; ac++)
            {
                _output[inOutIndex] = (input[inOutIndex] - _mean[valueIndex]) / (std::sqrt(_variance[valueIndex]) + _epsilon);
                inOutIndex++;
            }
            valueIndex++;
        }
        return _output;
    }

    void BatchNormalizationLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["applyCount"] << _applyCount;

        auto temp = _mean.ToArray();
        archiver["mean"] << temp;
        temp = _variance.ToArray();
        archiver["variance"] << temp;
        temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void BatchNormalizationLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["applyCount"] >> _applyCount;

        std::vector<double> temp;
        archiver["mean"] >> temp;
        _mean = ILayer::LayerVector(std::move(temp));
        archiver["variance"] >> temp;
        _variance = ILayer::LayerVector(std::move(temp));
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

