////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayer.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ActivationLayer.h"

// stl
#include <algorithm>

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ActivationFunctionType>
    ILayer::LayerVector& ActivationLayer<ActivationFunctionType>::FeedForward(const ILayer::LayerVector& input)
    {
        // Apply the activation function for each element
        _activation.Apply(input, _output);
        
        return _output;
    }

    template <typename ActivationFunctionType>
    void ActivationLayer<ActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        std::vector<double> temp = _output.ToArray();
        archiver["output"] << temp;
    }

    template <typename ActivationFunctionType>
    void ActivationLayer<ActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<double> temp;
        archiver["output"] >> temp;
        _output = ILayer::LayerVector(temp);
    }

}
}
}
