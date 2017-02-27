////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ILayer.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> A layer in a neural network that implements a fully connected layer, meaning all nodes in this layer are connected to all
    /// outputs of the previous layer (which are the inputs of this layer). </summary>
    class FullyConnectedLayer : public ILayer
    {
    public:

        /// <summary> Instantiates an instance of a fully connected layer. </summary>
        ///
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of outputs. </param>
        /// <param name="numInputsPerNode"> The number of inputs per node. For this layer type, it should equal the number of outputs of the previous layer. </param>
        /// <param name="weights"> The set of weights to apply, in row order. The number of weights should be numNodes * numInputsPerNode. </param>
        FullyConnectedLayer(size_t numNodes, size_t numInputsPerNode, const std::vector<double>& weights);

        /// <summary> Instantiates an instance of a fully connected layer. </summary>
        ///
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of outputs. </param>
        /// <param name="numInputsPerNode"> The number of inputs per node. For this layer type, it should equal the number of outputs of the previous layer. </param>
        /// <param name="weights"> The set of weights to apply, in row order. The number of weights should be numNodes * numInputsPerNode. </param>
        FullyConnectedLayer(size_t numNodes, size_t numInputsPerNode, std::vector<double>&& weights);

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
        size_t NumInputs() const override { return _numInputs; }

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
        size_t _numInputs;
        LayerMatrix _weights;
        LayerVector _output;
    };

}
}
}

