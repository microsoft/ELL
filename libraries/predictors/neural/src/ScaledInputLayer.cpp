////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScaledInputLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ScaledInputLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    ScaledInputLayer::ScaledInputLayer(size_t numNodes, double scaleFactor) :
        _output(numNodes),
        _scale(scaleFactor)
    {
    }

    ILayer::LayerVector& ScaledInputLayer::FeedForward(const ILayer::LayerVector& input)
    {
        _output = input;
        math::Operations::Multiply(_scale, _output);
        return _output;
    }

    void ScaledInputLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["scale"] << _scale;
        auto temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void ScaledInputLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["scale"] >> _scale;
        std::vector<double> temp;
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

