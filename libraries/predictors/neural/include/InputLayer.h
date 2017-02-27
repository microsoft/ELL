////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputLayer.h (neural)
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
    /// <summary> A layer in a neural network that implements an input layer, meaning input of the nodes is simply passed to the output. </summary>
    class InputLayer : public ILayer
    {
    public:

        /// <summary> Instantiates an instance of an input layer. </summary>
        ///
        /// <param name="numNodes"> The number of nodes in the layer. For this layer type, it is equivalent to the number of inputs and outputs. </param>
        InputLayer(size_t numNodes) : _output(numNodes) { };

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
        LayerVector _output;
    };

}
}
}

