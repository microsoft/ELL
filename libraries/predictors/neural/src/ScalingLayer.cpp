////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ScalingLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    ScalingLayer::ScalingLayer(size_t numNodes, std::vector<double>& scales, size_t applyCount) :
        _applyCount(applyCount),
        _scales(std::move(scales)),
        _output(numNodes)
    {
        if ((_scales.Size() * applyCount) != numNodes)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "scales dimension times apply count should be the same as number of nodes");
        }
    }

    ILayer::LayerVector& ScalingLayer::FeedForward(const ILayer::LayerVector& input)
    {
        size_t inOutIndex = 0;
        size_t scaleIndex = 0;
        while (inOutIndex < input.Size() && inOutIndex < _output.Size())
        {
            for (size_t ac = 0; ac < _applyCount; ac++)
            {
                _output[inOutIndex] = input[inOutIndex] * _scales[scaleIndex];
                inOutIndex++;
            }
            scaleIndex++;
        }
        return _output;
    }

    void ScalingLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["applyCount"] << _applyCount;

        auto temp = _scales.ToArray();
        archiver["scales"] << temp;
        temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void ScalingLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["applyCount"] >> _applyCount;

        std::vector<double> temp;
        archiver["scales"] >> temp;
        _scales = ILayer::LayerVector(std::move(temp));
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

