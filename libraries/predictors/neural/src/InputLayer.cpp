////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    ILayer::LayerVector& InputLayer::FeedForward(const ILayer::LayerVector& input)
    {
        _output = input;
        return _output;
    }

    void InputLayer::WriteToArchive(utilities::Archiver& archiver) const
    {
        auto temp = _output.ToArray();
        archiver["output"] << temp;
    }

    void InputLayer::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<double> temp;
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(std::move(temp));
    }

}
}
}

