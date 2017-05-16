////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputLayer.tcc (neural)
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

    template <typename ElementType>
    InputLayer<ElementType>::InputLayer(const InputParameters& inputParameters) : 
        Layer<ElementType>(LayerParameters{TensorType(1, 1, 1), inputParameters.inputPaddingParameters, inputParameters.outputShape, inputParameters.outputPaddingParameters}), _data(inputParameters.inputShape),
        _scale(NumOutputChannels())
    { 
        _layerParameters.input = _data; 
        _scale.Fill(inputParameters.scale);
    }

    template <typename ElementType>
    void InputLayer<ElementType>::SetInput(const DataVectorType& input)
    {
        size_t index = 0;
        auto& inputTensor = _data;

        for (size_t i = 0; i < inputTensor.NumRows(); ++i)
        {
            for (size_t j = 0; j < inputTensor.NumColumns(); ++j)
            {
                for (size_t k = 0; k < inputTensor.NumChannels(); ++k)
                {
                    inputTensor(i, j, k) = static_cast<ElementType>(input[index++]);
                }
            }
        }
    }

    template <typename ElementType>
    void InputLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        AssignValues(input, output);
        math::TensorOperations::Multiply<math::Dimension::channel>(_scale, output);
    }

    template <typename ElementType>
    void InputLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        // TODO:
    }

    template <typename ElementType>
    void InputLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        // TODO:
    }

}
}
}

