///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegionDetectionLayer.tcc (neural)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#include "SoftMaxActivation.h"
#include "SigmoidActivation.h"

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    RegionDetectionLayer<ElementType>::RegionDetectionLayer(const LayerParameters& layerParameters, RegionDetectionParameters regionDetectionParams) :
        Base(layerParameters), _regionDetectionParams(std::move(regionDetectionParams))
    {
        if (_regionDetectionParams.numCoordinates != 4)
        {
            throw std::invalid_argument("regionDetectionParams.numCoordinates.numCoordinates != 4");
        }

        if (_regionDetectionParams.width <= 0)
        {
            throw std::invalid_argument("regionDetectionParams.width <= 0");
        }

        if (_regionDetectionParams.height <= 0)
        {
            throw std::invalid_argument("regionDetectionParams.height <= 0");
        }

        if (_regionDetectionParams.numBoxesPerCell <= 0)
        {
            throw std::invalid_argument("regionDetectionParams.numBoxesPerCell <= 0");
        }

        if (_regionDetectionParams.numClasses <= 0)
        {
            throw std::invalid_argument("regionDetectionParams.numClasses <= 0");
        }

        if (this->_layerParameters.input.NumRows() != (size_t)_regionDetectionParams.width)
        {
            throw std::invalid_argument("input number of rows doesn't match width in detection parameters");
        }

        if (this->_layerParameters.input.NumColumns() != (size_t)_regionDetectionParams.height)
        {
            throw std::invalid_argument("input number of columns doesn't match height in detection parameters");
        }

        if (this->_layerParameters.input.NumChannels() != (size_t)((
                        (_regionDetectionParams.numCoordinates + 1 + _regionDetectionParams.numClasses) *
                        _regionDetectionParams.numBoxesPerCell)))
        {
            throw std::invalid_argument("input number of channels doesn't match box size * number of boxes in detection parameters");
        }
    }

    template <typename ElementType>
    void RegionDetectionLayer<ElementType>::Compute()
    {
        auto output = this->GetOutputMinusPadding();
        auto& input = this->_layerParameters.input;

        assert(output.GetShape() == input.GetShape());

        // The input has the shape of width x height x ((5 + classes) * numBoxes)
        // Each "cell" in the third dimension has the format
        // [tx, ty, tw, th, tc, class probabilities...] for each "box".
        // The first four (tx, ty, tw, th) are coordinates that define
        // the bounding box for the region where the network thinks an
        // object might be. tc is the confidence on the presence of an
        // object at all, and should be considered the scale of the
        // class probabilities.

        SigmoidActivation<ElementType> sigmoid;
        SoftMaxActivation<ElementType> softmax;

        for (int i = 0; i < _regionDetectionParams.width; ++i)
        {
            for (int j = 0; j < _regionDetectionParams.height; ++j)
            {
                auto outputChannelVector = output.template GetSlice<math::Dimension::channel>(i, j);
                auto inputChannelVector = input.template GetSlice<math::Dimension::channel>(i, j);

                for (int k = 0; k < _regionDetectionParams.numBoxesPerCell; ++k)
                {
                    auto boxOffset = k * (_regionDetectionParams.numCoordinates + 1 + _regionDetectionParams.numClasses);

                    // Apply sigmoid function to tx and ty
                    outputChannelVector[boxOffset + 0] = sigmoid(inputChannelVector[boxOffset + 0]);
                    outputChannelVector[boxOffset + 1] = sigmoid(inputChannelVector[boxOffset + 1]);

                    // Apply exp function to tw and th
                    outputChannelVector[boxOffset + 2] = std::exp(inputChannelVector[boxOffset + 2]);
                    outputChannelVector[boxOffset + 3] = std::exp(inputChannelVector[boxOffset + 3]);

                    // Apply sigmoid function to tc
                    outputChannelVector[boxOffset + 4] = sigmoid(inputChannelVector[boxOffset + 4]);

                    // Get the vector for the class probabilities for both output and input
                    auto outputClassProbabilities = outputChannelVector.GetSubVector(boxOffset + 5, _regionDetectionParams.numClasses);
                    auto inputClassProbabilities = inputChannelVector.GetSubVector(boxOffset + 5, _regionDetectionParams.numClasses);

                    // Copy input over to output
                    outputClassProbabilities.CopyFrom(inputClassProbabilities);

                    // Apply softmax to probabilities
                    softmax(outputClassProbabilities);
                }
            }
        }
    }

    template <typename ElementType>
    void RegionDetectionLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);

        archiver["width"] << _regionDetectionParams.width;
        archiver["height"] << _regionDetectionParams.height;
        archiver["numBoxesPerCell"] << _regionDetectionParams.numBoxesPerCell;
        archiver["numClasses"] << _regionDetectionParams.numClasses;
        archiver["numCoordinates"] << _regionDetectionParams.numCoordinates;
    }

    template <typename ElementType>
    void RegionDetectionLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& unarchiver)
    {
        Layer<ElementType>::ReadFromArchive(unarchiver);

        unarchiver["width"] >> _regionDetectionParams.width;
        unarchiver["height"] >> _regionDetectionParams.height;
        unarchiver["numBoxesPerCell"] >> _regionDetectionParams.numBoxesPerCell;
        unarchiver["numClasses"] >> _regionDetectionParams.numClasses;
        unarchiver["numCoordinates"] >> _regionDetectionParams.numCoordinates;
    }
}
}
}
