////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ILayer.h"

// math
#include "Matrix.h"

// stl
#include <vector>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
    struct PoolingParameters
    {
        /// <summary> Width of the input. </summary>
        size_t width;

        /// <summary> Height of the input. </summary>
        size_t height;

        /// <summary> Depth of the input. </summary>
        size_t depth;

        /// <summary> Width and height of the pooling field that is slid over the input. </summary>
        size_t poolingSize;

        /// <summary> Number of elements to move/jump when sliding over the input. Often this is the same as poolingSize. </summary>
        size_t stride;

        /// <summary> Amount of zero padding to use on the input to ensure the output is of the proper size. Often this is 0. </summary>
        size_t padding;
    };

    /// <summary> A layer in a neural network that implements pooling. </summary>
    template <typename PoolingFunctionType>
    class PoolingLayer : public ILayer
    {
    public:

        /// <summary> Instantiates an instance of a pooling layer. </summary>
        ///
        /// <param name="poolingParameters"> Specifies the input and pooling characteristics of the layer. </param>
        PoolingLayer(PoolingParameters poolingParameters);

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
        size_t NumInputs() const override;

        /// <summary> Returns the size of the output vector. </summary>
        ///
        /// <returns> Size of the output vector. </returns>
        size_t NumOutputs() const override;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        size_t _outputWidth;
        size_t _outputHeight;
        PoolingParameters _poolingParameters;
        LayerVector _output;
    };

}
}
}

#include "../tcc/PoolingLayer.tcc"