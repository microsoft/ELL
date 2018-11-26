///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegionDetectionLayer.h (neural)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "SigmoidActivation.h"
#include "SoftMaxActivation.h"

#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Specifies the hyper parameters of the region detection layer. </summary>
        struct RegionDetectionParameters
        {
            /// <summary> Width of the input. </summary>
            int width = 0;

            /// <summary> Height of the input. </summary>
            int height = 0;

            /// <summary> Number of possible bounding boxes per cell. </summary>
            int numBoxesPerCell = 0;

            /// <summary> Number of classes that can be detected. </summary>
            int numClasses = 0;

            /// <summary> Number of anchors per region. Typically this is 4  </summary>
            int numAnchors = 0;

            /// <summary> Specifies whether softmax should be applied to class probabilites </summary>
            bool applySoftmax = false;
        };

        /// <summary> A layer in a neural network that is used to predict the region (e.g. bounding box) of detection classes. </summary>
        template <typename ElementType>
        class RegionDetectionLayer : public Layer<ElementType>
        {
        public:
            using Base = Layer<ElementType>;
            using LayerParameters = typename Layer<ElementType>::LayerParameters;

            /// <summary> Instantiates an instance of a region detection layer. </summary>
            ///
            /// <param name="regionDetectionParams"> The parameters desribing the configuration of this layer. </param>
            /// <param name="anchorScales"> The values to scale the width and height for each of the anchor regions.
            /// The number of elements should equal 2 x numBoxesPerCell. </param>
            RegionDetectionLayer(const LayerParameters& layerParameters, RegionDetectionParameters regionDetectionParamss);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            RegionDetectionLayer() :
                _regionDetectionParams({}) {}

            /// <summary> Feeds the input forward through the layer </summary>
            void Compute() override;

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("RegionDetectionLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Gets the parameters for the region detection. </summary>
            ///
            /// <returns> The structure defining the parameters for the region detection. </returns>
            const RegionDetectionParameters& GetDetectionParameters() const { return _regionDetectionParams; }

        protected:
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            RegionDetectionParameters _regionDetectionParams;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        RegionDetectionLayer<ElementType>::RegionDetectionLayer(const LayerParameters& layerParameters, RegionDetectionParameters regionDetectionParams) :
            Base(layerParameters),
            _regionDetectionParams(std::move(regionDetectionParams))
        {
            if (_regionDetectionParams.numAnchors <= 0)
            {
                throw std::invalid_argument("regionDetectionParams.numAnchors <= 0");
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
                                                                  (_regionDetectionParams.numAnchors + 1 + _regionDetectionParams.numClasses) *
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
            auto numAnchors = _regionDetectionParams.numAnchors;

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
                        auto boxOffset = k * (numAnchors + 1 + _regionDetectionParams.numClasses);

                        // Get the vector for the anchors for both output and input
                        auto outputAnchors = outputChannelVector.GetSubVector(boxOffset, numAnchors);
                        auto inputAnchors = inputChannelVector.GetSubVector(boxOffset, numAnchors);

                        // Copy input over to output
                        outputAnchors.CopyFrom(inputAnchors);

                        // Apply sigmoid to the confidence value, which is immediately after the anchor points
                        outputChannelVector[boxOffset + numAnchors] = sigmoid(inputChannelVector[boxOffset + numAnchors]);

                        // Get the vector for the class probabilities for both output and input
                        auto outputClassProbabilities = outputChannelVector.GetSubVector(boxOffset + numAnchors + 1, _regionDetectionParams.numClasses);
                        auto inputClassProbabilities = inputChannelVector.GetSubVector(boxOffset + numAnchors + 1, _regionDetectionParams.numClasses);

                        // Copy input over to output
                        outputClassProbabilities.CopyFrom(inputClassProbabilities);

                        if (_regionDetectionParams.applySoftmax)
                        {
                            // Apply softmax to probabilities
                            softmax(outputClassProbabilities);
                        }
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
            archiver["numCoordinates"] << _regionDetectionParams.numAnchors;
        }

        template <typename ElementType>
        void RegionDetectionLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& unarchiver)
        {
            Layer<ElementType>::ReadFromArchive(unarchiver);

            unarchiver["width"] >> _regionDetectionParams.width;
            unarchiver["height"] >> _regionDetectionParams.height;
            unarchiver["numBoxesPerCell"] >> _regionDetectionParams.numBoxesPerCell;
            unarchiver["numClasses"] >> _regionDetectionParams.numClasses;
            unarchiver["numCoordinates"] >> _regionDetectionParams.numAnchors;
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
