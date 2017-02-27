////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BiasLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    BiasLayer::BiasLayer(size_t numNodes, const std::vector<double>& bias, size_t applyCount) :
        _applyCount(applyCount),
        _bias(bias),
        _output(numNodes)
    {
        if ((_bias.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "bias dimension times apply count should be the same as number of nodes");
        }
    }

    BiasLayer::BiasLayer(size_t numNodes, std::vector<double>&& bias, size_t applyCount) :
        _applyCount(applyCount),
        _bias(std::move(bias)),
        _output(numNodes)
    {
        if ((_bias.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "bias dimension times apply count should be the same as number of nodes");
        }
    }

    ILayer::LayerVector& BiasLayer::FeedForward(const ILayer::LayerVector& input)
    {
        size_t inOutIndex = 0;
        size_t biasIndex = 0;
        while (inOutIndex < input.Size() && inOutIndex < _output.Size())
        {
            for (size_t ac = 0; ac < _applyCount; ac++)
            {
                _output[inOutIndex] = input[inOutIndex] + _bias[biasIndex];
                inOutIndex++;
            }
            biasIndex++;
        }
        return _output;
    }

    void BiasLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["applyCount"] << _applyCount;

        auto temp = _bias.ToArray();
        archiver["bias"] << temp;
        temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void BiasLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["applyCount"] >> _applyCount;

        std::vector<double> temp;
        archiver["bias"] >> temp;
        _bias = ILayer::LayerVector(std::move(temp));
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

