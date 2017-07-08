////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        if (this->GetInputShape() != this->GetOutputShapeMinusPadding())
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
        Layer<ElementType>::WriteToArchive(archiver);

        math::VectorArchiver::Write(_bias, "bias", archiver);
    }

    template <typename ElementType>
    void BiasLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        math::VectorArchiver::Read(_bias, "bias", archiver);
    }

}
}
}

