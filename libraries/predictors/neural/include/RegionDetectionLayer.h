///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegionDetectionLayer.h (neural)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"

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
        int width;

        /// <summary> Height of the input. </summary>
        int height;

        /// <summary> Number of possible bounding boxes per cell. </summary>
        int numBoxesPerCell;

        /// <summary> Number of classes that can be detected. </summary>
        int numClasses;

        /// <summary> Number of coordinates per region. Typically this is 4  </summary>
        int numCoordinates;
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
        RegionDetectionLayer() : _regionDetectionParams({}) {}

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
}
}
}

#include "../tcc/RegionDetectionLayer.tcc"

