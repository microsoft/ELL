////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.h (neural)
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
    /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
    struct ConvolutionalParameters
    {
        /// <summary> Width of the input. </summary>
        size_t width;

        /// <summary> Height of the input. </summary>
        size_t height;

        /// <summary> Depth of the input. </summary>
        size_t depth;

        /// <summary> Width and height of the receptive field that is slid over the input. </summary>
        size_t receptiveField;

        /// <summary> Number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </summary>
        size_t stride;

        /// <summary> Amount of zero padding to use on the input to ensure the output is of the proper size. </summary>
        size_t padding;

        /// <summary> Number of kernels or filters of the convolutional layer. The output will have this depth. </summary>
        size_t numFilters;
    };

    /// <summary> A layer in a neural network that implements a fully connected layer, meaning all nodes in this layer are connected to all
    /// outputs of the previous layer (which are the inputs of this layer). </summary>
    class ConvolutionalLayer : public ILayer
    {
        /// <summary> The matrix type for matrix operations on the input to a convolutional layer. </summary>
        using InputMatrix = math::RowMatrix<double>;

    public:

        /// <summary> Instantiates an instance of a convolutional layer. </summary>
        ///
        /// <param name="inputDimensions"> Specifies the dimensional characteristics of the input volume. </param>
        /// <param name="convolutionalParameters"> The hyperparameters for this convolutional layer. </param>
        /// <param name="weights"> The set of weights to apply, in row then depth order. The number of weights should be (inputDimensions.width * inputDimensions.height * inputDimensions.depth * convolutionalParameters.numFilters). </param>
        /// <param name="bias"> The set of bias to apply to each filter. The number of bias values should be convolutionalParameters.numFilters. </param>
        ConvolutionalLayer(ConvolutionalParameters convolutionalParameters, const std::vector<double>& weights, const std::vector<double>& bias);

        /// <summary> Instantiates an instance of a convolutional layer. </summary>
        ///
        /// <param name="inputDimensions"> Specifies the dimensional characteristics of the input volume. </param>
        /// <param name="convolutionalParameters"> The hyperparameters for this convolutional layer. </param>
        /// <param name="weights"> The set of weights to apply, in row then depth order. The number of weights should be (inputDimensions.width * inputDimensions.height * inputDimensions.depth * convolutionalParameters.numFilters). </param>
        /// <param name="bias"> The set of bias to apply to each filter. The number of bias values should be convolutionalParameters.numFilters. </param>
        ConvolutionalLayer(ConvolutionalParameters convolutionalParameters, std::vector<double>&& weights, std::vector<double>&& bias);

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
        //
        // private member functions
        //

        // Returns a value from a volume. The volume is backed by an array whose dimensions are volumeWidth x volumeHeight x volumeDepth.
        inline static double GetValueFromVolume(const double* inputVolume, const ConvolutionalParameters& convolutionalParameters, intptr_t valueRow, intptr_t valueCol, intptr_t valueDepth);

        // Fills a matrix (backed by the array outputMatrix) where the columns are the values of the receptive from the input, streched into a column vector.
        // The number of columns is equal to the number of locations that a receptive field is slid over the input volume.
        void ReceptiveFieldToColumns(double* inputVolume, const ConvolutionalParameters& convolutionalParameters, double* outputMatrix);

        //
        //  member variables
        //
        ConvolutionalParameters _convolutionalParameters;
        size_t _fieldVolumeSize;
        size_t _outputWidth;
        size_t _outputHeight;
        size_t _numNeuronsPerFilter;
        LayerMatrix _weights;
        InputMatrix _shapedInput;
        LayerVector _bias;
        LayerVector _output;
        LayerReferenceMatrix _outputMatrixReference;
    };

}
}
}

