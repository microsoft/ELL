////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingLayer.tcc (neural)
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
    ScalingLayer<ElementType>::ScalingLayer(const LayerParameters& layerParameters, const VectorType& scales) :
        Layer<ElementType>(layerParameters),
        _scales(scales)
    {
    }

    template <typename ElementType>
    void ScalingLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        AssignValues(input, output);
        math::ScaleUpdate<math::Dimension::channel>(_scales, output);
    }

    template <typename ElementType>
    void ScalingLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);

        math::VectorArchiver::Write(_scales, "scales", archiver);
    }

    template <typename ElementType>
    void ScalingLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        math::VectorArchiver::Read(_scales, "scales", archiver);
    }
}
}
}

