////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ILayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> A layer in a neural network that applies batch normalization to the input. </summary>
    class BatchNormalizationLayer : public ILayer
    {
    public:

        /// <summary> Instantiates an instance of a batch normalization layer. </summary>
        ///
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of inputs and outputs. </param>
        /// <param name="mean"> The mean values. </param>
        /// <param name="mean"> The variance values. </param>
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of inputs and outputs. </param>
        /// <param name="applyCount"> The number of times to apply each mean and variance value. Often, there is one of each for every input value, but not always.
        /// For example in a convolutional layer, there may be one value for every input value in the same filter. In that case, the 
        /// applyCount equals the number of neurons per fitler.
        BatchNormalizationLayer(size_t numNodes, const std::vector<double>& mean, const std::vector<double>& variance, size_t applyCount = 1);

        /// <summary> Instantiates an instance of a batch normalization layer. </summary>
        ///
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of inputs and outputs. </param>
        /// <param name="mean"> The mean values. </param>
        /// <param name="mean"> The variance values. </param>
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of inputs and outputs. </param>
        /// <param name="applyCount"> The number of times to apply each mean and variance value. Often, there is one of each for every input value, but not always.
        /// For example in a convolutional layer, there may be one value for every input value in the same filter. In that case, the 
        /// applyCount equals the number of neurons per fitler.
        BatchNormalizationLayer(size_t numNodes, std::vector<double>&& mean, std::vector<double>&& variance, size_t applyCount = 1);

        /// <summary> Feeds the input forward throught the layer and returns a reference to the output. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> A reference to the output vector. </returns>
        LayerVector& FeedForward(const LayerVector& input) override;

        /// <summary> Returns a reference to the output values, which is the result after the last #Forward call. </summary>
        ///
        /// <returns> A reference to the output vector. </returns>
        LayerVector& GetOutput() override { return _output; }

        /// <summary> Returns the expected size of the input vector. </summary>
        ///
        /// <returns> Expected size of the input vector. </returns>
        size_t NumInputs() const override { return _output.Size(); }

        /// <summary> Returns the size of the output vector. </summary>
        ///
        /// <returns> Size of the output vector. </returns>
        size_t NumOutputs() const override { return _output.Size(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        size_t _applyCount;
        LayerVector _mean;
        LayerVector _variance;
        LayerVector _output;
        const double _epsilon = 0.000001; // To ensure non-zero division, this is added to denominator
    };

}
}
}

