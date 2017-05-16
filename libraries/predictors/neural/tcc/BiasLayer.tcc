////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayer.tcc (neural)
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

    template <typename ElementType>
    BiasLayer<ElementType>::BiasLayer(const LayerParameters& layerParameters, const VectorType& bias) :
        Layer<ElementType>(layerParameters),
        _bias(bias)
    {        
        if (_layerParameters.input.NumElements() != GetOutputMinusPadding().NumElements())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Expected size of input and output tensor (minus padding) to match");
        }
        if (_bias.Size() != NumOutputChannels())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Number of 'bias' values must equal number of channels in output");
        }
    }

    template <typename ElementType>
    void BiasLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto input = _layerParameters.input;

        AssignValues(input, output);
        math::TensorOperations::Add<math::Dimension::channel>(_bias, output);
    }

    template <typename ElementType>
    void BiasLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        // TODO:
    }

    template <typename ElementType>
    void BiasLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        // TODO:
    }

}
}
}

