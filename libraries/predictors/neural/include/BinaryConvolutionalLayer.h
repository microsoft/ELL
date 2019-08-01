////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ConvolutionalLayer.h"
#include "Layer.h"

#include <math/include/Matrix.h>

#include <utilities/include/TypeAliases.h>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> The method for performing binary convolutions. </summary>
        enum class BinaryConvolutionMethod : int
        {
            /// <summary> Perform the binary convolution as a real-valued GEMM operation (e.g. values are -mean and mean). </summary>
            gemm = 0,
            /// <summary> Perform binary convolution as bitwise operations. </summary>
            bitwise = 1,
        };

        /// <summary> The scale to apply to the binarized weights. </summary>
        enum class BinaryWeightsScale : int
        {
            /// <summary> Perform no scaling of the binarized weights. </summary>
            none = 0,
            /// <summary> Scale the binarized weights by their mean. </summary>
            mean = 1,
        };

        /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
        struct BinaryConvolutionalParameters
        {
            /// <summary> Width and height of the receptive field that is slid over the input. </summary>
            size_t receptiveField;

            /// <summary> Number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </summary>
            size_t stride;

            /// <summary> Method for doing convolution. </summary>
            BinaryConvolutionMethod method;

            /// <summary The scaling to apply to the binarized weights </summary>
            BinaryWeightsScale weightsScale;
        };

        /// <summary> A layer in a neural network that implements a binarized convolutional layer, where operations will occur
        /// on binarized input with binarized weights. </summary>
        template <typename ElementType>
        class BinaryConvolutionalLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using MatrixType = typename Layer<ElementType>::MatrixType;
            using TensorType = typename Layer<ElementType>::TensorType;
            using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::NumOutputRowsMinusPadding;
            using Layer<ElementType>::NumOutputColumnsMinusPadding;
            using Layer<ElementType>::NumOutputChannels;

            /// <summary> Instantiates an instance of a binarized convolutional layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            /// <param name="convolutionalParameters"> The hyperparameters for this convolutional layer. </param>
            /// <param name="weights"> The set of weights to apply. </param>
            BinaryConvolutionalLayer(const LayerParameters& layerParameters, const BinaryConvolutionalParameters& convolutionalParameters, const ConstTensorReferenceType& weights);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            BinaryConvolutionalLayer() :
                _realValuedShapedInputMatrix(0, 0),
                _realValuedWeightsMatrix(0, 0),
                _realValuedOutputMatrix(0, 0) {}

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::binaryConvolution; }

            /// <summary> Get the parameters used to control convolution. </summary>
            ///
            /// <returns> A BinaryConvolutionalParameters struct. </returns>
            const BinaryConvolutionalParameters& GetConvolutionalParameters() const { return _convolutionalParameters; }

            /// <summary> Get the weights for the convolution filters. </summary>
            ///
            /// <returns> The weights, packed into a Tensor. </returns>
            const MatrixType& GetRealFilterWeights() const { return _realValuedWeightsMatrix; }

            /// <summary> Get the weights for the convolution filters, packed as bits. </summary>
            ///
            /// <returns> The weights, packed as bits. </returns>
            const std::vector<std::vector<uint64_t>> GetCompressedFilterWeights() const { return _binarizedWeights; }

            /// <summary> Get the means for the convolution filters. </summary>
            ///
            /// <returns> The means of the convolution filters. </returns>
            const std::vector<ElementType>& GetFilterMeans() const { return _filterMeans; }

            /// <summary> Get the input padding masks, packed as bits. </summary>
            ///
            /// <returns> The padding masks, packed as bits. </returns>
            const std::vector<std::vector<uint64_t>> GetCompressedInputPaddingMasks() const { return _shapedInputPaddingMask; }

            /// <summary> Get the input padding mask sums. </summary>
            ///
            /// <returns> The padding masks, packed as bits. </returns>
            const std::vector<int> GetInputPaddingMaskSums() const { return _shapedInputPaddingMaskSums; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("BinaryConvolutionalLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        protected:
            void WriteToArchive(utilities::Archiver& archiver) const override;
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            // Fills a vector of vectors where each row is the set of input values corresponding to a filter, stretched into a vector.
            // The number of vectors is equal to the number of locations that the filter is slid over the input tensor.
            void ReceptiveFieldToBinaryRows(ConstTensorReferenceType input, std::vector<std::vector<uint64_t>>& shapedInput);

            // Fills a matrix (backed by the array outputMatrix) where the columns the set of input values corresponding to a filter, stretched into a vector.
            // The number of columns is equal to the number of locations that a filter is slide over the input tensor.
            void ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput);

            // Returns whether input zero padding is enabled
            bool HasInputZeroPadding() const;

            // Returns whether the row, column indices correspond to input zero padding
            bool IsInputZeroPadding(size_t row, size_t column) const;

            void ComputeWeightsMatrices(const ConstTensorReferenceType& weights);
            void ComputeRealValuedWeightsMatrix();
            void ComputeShapedInputPaddingMask();
            void InitializeIOMatrices();

            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;

            using IntPtrT = utilities::IntPtrT;

            constexpr static size_t _binaryElementSize = 64;
            BinaryConvolutionalParameters _convolutionalParameters;
            std::vector<std::vector<uint64_t>> _binarizedShapedInput;
            std::vector<std::vector<uint64_t>> _binarizedWeights;
            std::vector<std::vector<uint64_t>> _shapedInputPaddingMask;
            std::vector<int> _shapedInputPaddingMaskSums;
            std::vector<ElementType> _filterMeans;

            MatrixType _realValuedShapedInputMatrix;
            MatrixType _realValuedWeightsMatrix;
            MatrixType _realValuedOutputMatrix;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

// TODO: let's make a popcount function that does the right thing
#if defined(_MSC_VER)
#include <intrin.h>
#define POPCOUNT64 __popcnt64
#else
#define POPCOUNT64 __builtin_popcountl
#endif

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        BinaryConvolutionalLayer<ElementType>::BinaryConvolutionalLayer(const LayerParameters& layerParameters, const BinaryConvolutionalParameters& convolutionalParameters, const ConstTensorReferenceType& weights) :
            Layer<ElementType>(layerParameters),
            _convolutionalParameters(convolutionalParameters),
            _realValuedShapedInputMatrix(0, 0),
            _realValuedWeightsMatrix(0, 0),
            _realValuedOutputMatrix(0, 0)
        {
            if (weights.GetConstDataPointer() == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "weights tensor has null data field");
            }

            if (weights.Size() != (NumOutputChannels() * _layerParameters.input.NumChannels() * convolutionalParameters.receptiveField * convolutionalParameters.receptiveField))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "weights dimensions for a convolutional layer should be the size of the receptive field volume * number of filters");
            }

            ComputeWeightsMatrices(weights);
            InitializeIOMatrices();
            ComputeShapedInputPaddingMask();
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ComputeWeightsMatrices(const ConstTensorReferenceType& weights)
        {
            const auto filterWidth = _convolutionalParameters.receptiveField;

            _binarizedWeights.resize(NumOutputChannels());
            _filterMeans.resize(NumOutputChannels());
            _realValuedWeightsMatrix = MatrixType(NumOutputChannels(), filterWidth * filterWidth * _layerParameters.input.NumChannels());

            const size_t binarizedFilterVolumeSize = ((filterWidth * filterWidth * _layerParameters.input.NumChannels()) + (_binaryElementSize - 1)) / _binaryElementSize;

            // Binarize the weights and calculate the mean per filter
            auto flattened = weights.ReferenceAsMatrix();
            for (size_t startRow = 0; startRow < flattened.NumRows() / filterWidth; ++startRow)
            {
                // Iterate over the weights corresponding to the filter and calculate the mean
                ElementType sum = 0;
                std::vector<ElementType> filterWeights(filterWidth * filterWidth * _layerParameters.input.NumChannels());
                for (size_t row = 0; row < filterWidth; row++)
                {
                    auto weightsVector = flattened.GetMajorVector(startRow * filterWidth + row);

                    for (size_t i = 0; i < weightsVector.Size(); ++i)
                    {
                        const size_t columnOffset = row * weightsVector.Size();
                        ElementType value = weightsVector[i];

                        sum += std::abs(value);
                        filterWeights[columnOffset + i] = value;
                    }
                }

                ElementType mean = sum / static_cast<ElementType>(filterWeights.size());
                _filterMeans[startRow] = mean;

                // initialize the mean according to the binary weights scale
                ElementType scale(1.0);
                if (_convolutionalParameters.weightsScale == BinaryWeightsScale::mean)
                {
                    scale = mean;
                }

                for (size_t i = 0; i < filterWeights.size(); ++i)
                {
                    // Set the weights matrix based on the weights value and mean
                    _realValuedWeightsMatrix(startRow, i) = (filterWeights[i] > 0) ? scale : -scale;
                }

                // Binarize and pack the weights
                _binarizedWeights[startRow].resize(binarizedFilterVolumeSize, 0);
                for (size_t i = 0; i < filterWeights.size(); ++i)
                {
                    size_t block = i / _binaryElementSize;
                    int bit = i % _binaryElementSize;
                    if (filterWeights[i] > 0)
                    {
                        _binarizedWeights[startRow][block] |= ((uint64_t)1 << bit);
                    }
                }
            }
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::InitializeIOMatrices()
        {
            const auto filterWidth = _convolutionalParameters.receptiveField;
            const auto outputShape = NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding();

            _realValuedShapedInputMatrix = { filterWidth * filterWidth * _layerParameters.input.NumChannels(), outputShape };
            _realValuedOutputMatrix = { NumOutputChannels(), outputShape };

            _binarizedShapedInput.resize(outputShape);
            _shapedInputPaddingMask.resize(outputShape);
            _shapedInputPaddingMaskSums.resize(outputShape);
            // Set the sizes of the shapedInput and padding mask vectors
            const size_t binarizedFilterVolumeSize = ((filterWidth * filterWidth * _layerParameters.input.NumChannels()) - 1) / _binaryElementSize + 1;
            for (size_t i = 0; i < _binarizedShapedInput.size(); ++i)
            {
                _binarizedShapedInput[i].resize(binarizedFilterVolumeSize, 0);
                _shapedInputPaddingMask[i].resize(binarizedFilterVolumeSize, 0);
            }
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto input = _layerParameters.input;

            if (_convolutionalParameters.method == BinaryConvolutionMethod::gemm)
            {
                // Re-shape input.
                ReceptiveFieldToColumns(input, _realValuedShapedInputMatrix);

                // Multiply reshaped input and weights.
                math::MultiplyScaleAddUpdate(static_cast<ElementType>(1.0), _realValuedWeightsMatrix, _realValuedShapedInputMatrix, static_cast<ElementType>(0.0), _realValuedOutputMatrix);

                // Re-shape the output into the output tensor
                for (size_t i = 0; i < output.NumRows(); ++i)
                {
                    for (size_t j = 0; j < output.NumColumns(); ++j)
                    {
                        for (size_t k = 0; k < output.NumChannels(); ++k)
                        {
                            size_t row = k;
                            size_t column = (i * output.NumColumns()) + j;
                            output(i, j, k) = _realValuedOutputMatrix(row, column);
                        }
                    }
                }
            }
            else
            {
                // Use the bitwise method
                // Binarize and pack the input
                ReceptiveFieldToBinaryRows(input, _binarizedShapedInput);

                // XOR and sum
                const size_t filterSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * input.NumChannels();
                const size_t binarizedFilterSize = _binarizedWeights[0].size();
                const size_t filterDrop = filterSize % _binaryElementSize;
                const size_t filterAdjust = _binaryElementSize - filterDrop;

                // Iterate over filters
                for (size_t i = 0; i < output.NumRows(); ++i)
                {
                    size_t shapedInputOffset = i * NumOutputColumnsMinusPadding();
                    for (size_t j = 0; j < output.NumColumns(); ++j)
                    {
                        for (size_t k = 0; k < output.NumChannels(); ++k)
                        {
                            ElementType sum = 0;

                            auto& binarizedWeights = _binarizedWeights[k];
                            auto& binarizedShapedInput = _binarizedShapedInput[shapedInputOffset + j];
                            auto& shapedInputPaddingMask = _shapedInputPaddingMask[shapedInputOffset + j];

                            for (size_t blockIndex = 0; blockIndex < binarizedFilterSize; blockIndex++)
                            {
                                const uint64_t fValue = binarizedWeights[blockIndex];
                                const uint64_t iValue = binarizedShapedInput[blockIndex];

                                if (HasInputZeroPadding())
                                {
                                    // Zeros are neither -1 nor 1, mask out the effects
                                    // of zero padding from the XOR product
                                    // This logic is only applied to zero padding where the effect
                                    // of inserting zeros is well-known, other padding
                                    // schemes that can generate zero values are not special-cased.
                                    const uint64_t maskValue = shapedInputPaddingMask[blockIndex];
                                    const uint64_t xorProduct = maskValue & (fValue ^ iValue);

                                    // Apply the actual zero padding, which is to "add back" the number of values
                                    // that were assumed to be -1
                                    sum += (2.0f * POPCOUNT64(xorProduct) - _binaryElementSize + POPCOUNT64(~maskValue));
                                }
                                else
                                {
                                    const uint64_t xorProduct = fValue ^ iValue;
                                    sum += (2.0f * POPCOUNT64(xorProduct) - _binaryElementSize);
                                }
                            }

                            ElementType scale(1.0);
                            if (_convolutionalParameters.weightsScale == BinaryWeightsScale::mean)
                            {
                                scale = _filterMeans[k];
                            }

                            if (filterDrop == 0)
                            {
                                output(i, j, k) = (-scale * sum);
                            }
                            else
                            {
                                output(i, j, k) = (-scale * (sum + filterAdjust));
                            }
                        }
                    }
                }
            }
        }

        // Fills a vector of vectors where each row is the values of the receptive field from the input stretched into a vector,
        // and the number of vectors is equal to the number of locations that a receptive field is slid over the input volume.
        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ReceptiveFieldToBinaryRows(ConstTensorReferenceType input, std::vector<std::vector<uint64_t>>& shapedInput)
        {
            const size_t fieldVolumeSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels();
            const size_t outputHeight = NumOutputRowsMinusPadding();
            const size_t outputWidth = NumOutputColumnsMinusPadding();
            const size_t rowMax = outputWidth * outputHeight;

            for (size_t outRow = 0; outRow < rowMax; ++outRow)
            {
                const size_t convolutionalRow = outRow / outputWidth;
                const size_t convolutionalCol = outRow % outputWidth;
                const size_t horizontalStart = (convolutionalCol * _convolutionalParameters.stride);
                const size_t verticalStart = (convolutionalRow * _convolutionalParameters.stride);

                for (size_t f = 0; f < fieldVolumeSize; ++f)
                {
                    // Calculate the col, row, depth values in the convolutional field volume
                    const size_t volDepth = f % input.NumChannels();
                    const size_t volCol = (f / input.NumChannels()) % _convolutionalParameters.receptiveField;
                    const size_t volRow = (f / input.NumChannels()) / _convolutionalParameters.receptiveField;

                    // Calculate where this fits in relation to the input volume
                    const IntPtrT sourceCol = horizontalStart + volCol;
                    const IntPtrT sourceRow = verticalStart + volRow;
                    const IntPtrT sourceDepth = volDepth;

                    ElementType value = input(sourceRow, sourceCol, sourceDepth);
                    const size_t block = (f / _binaryElementSize);
                    const size_t bit = f % _binaryElementSize;

                    if (bit == 0)
                    {
                        // Initialize to zero
                        shapedInput[outRow][block] = static_cast<uint64_t>(0);
                    }

                    // Set the bit value
                    if (value > 0)
                    {
                        shapedInput[outRow][block] += ((uint64_t)1 << bit);
                    }
                }
            }
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput)
        {
            const size_t fieldVolumeSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels();
            const size_t convolutionalHeight = NumOutputRowsMinusPadding();
            const size_t convolutionalWidth = NumOutputColumnsMinusPadding();

            for (size_t f = 0; f < fieldVolumeSize; ++f)
            {
                const size_t fieldDepth = f % _layerParameters.input.NumChannels();
                const size_t fieldColumn = (f / _layerParameters.input.NumChannels()) % _convolutionalParameters.receptiveField;
                const size_t fieldRow = (f / _layerParameters.input.NumChannels()) / _convolutionalParameters.receptiveField;

                size_t rowOffset = 0;
                for (size_t h = 0; h < convolutionalHeight; ++h)
                {
                    size_t colOffset = 0;
                    for (size_t w = 0; w < convolutionalWidth; ++w)
                    {
                        size_t inputRow = rowOffset + fieldRow;
                        size_t inputCol = colOffset + fieldColumn;

                        ElementType value = input(inputRow, inputCol, fieldDepth);

                        // Don't binarize zero-padded input when weights are not scaled
                        if (IsInputZeroPadding(inputRow, inputCol))
                        {
                            shapedInput(f, h * convolutionalWidth + w) = value;
                        }
                        else
                        {
                            shapedInput(f, h * convolutionalWidth + w) = (value > 0) ? 1.0f : -1.0f;
                        }

                        colOffset += _convolutionalParameters.stride;
                    }
                    rowOffset += _convolutionalParameters.stride;
                }
            }
        }

        template <typename ElementType>
        bool BinaryConvolutionalLayer<ElementType>::HasInputZeroPadding() const
        {
            return HasPadding(_layerParameters.inputPaddingParameters, PaddingScheme::zeros);
        }

        template <typename ElementType>
        bool BinaryConvolutionalLayer<ElementType>::IsInputZeroPadding(size_t row, size_t column) const
        {
            if (HasInputZeroPadding())
            {
                const size_t paddingSize = _layerParameters.inputPaddingParameters.paddingSize;
                const size_t rowPaddingRightIndex = _layerParameters.input.NumRows() - paddingSize;
                const size_t columnPaddingRightIndex = _layerParameters.input.NumColumns() - paddingSize;

                return row < paddingSize || row >= rowPaddingRightIndex ||
                       column < paddingSize || column >= columnPaddingRightIndex;
            }

            return false;
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            const size_t filterWidth = _convolutionalParameters.receptiveField;
            const size_t binarizedFilterVolumeSize = ((filterWidth * filterWidth * _layerParameters.input.NumChannels()) + (_binaryElementSize - 1)) / _binaryElementSize;

            Layer<ElementType>::WriteToArchive(archiver);

            archiver["receptiveField"] << _convolutionalParameters.receptiveField;
            archiver["stride"] << _convolutionalParameters.stride;
            archiver["method"] << static_cast<int>(_convolutionalParameters.method);
            archiver["weightsScale"] << static_cast<int>(_convolutionalParameters.weightsScale);

            // Compute binarized weights
            size_t numRows = _realValuedWeightsMatrix.NumRows();
            size_t numCols = _realValuedWeightsMatrix.NumColumns();
            std::vector<std::vector<uint64_t>> binarizedWeights(numRows);
            for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
            {
                binarizedWeights[rowIndex].resize(binarizedFilterVolumeSize, 0);
                for (size_t colIndex = 0; colIndex < numCols; ++colIndex)
                {
                    size_t block = colIndex / _binaryElementSize;
                    int bit = colIndex % _binaryElementSize;
                    if (_realValuedWeightsMatrix(rowIndex, colIndex) > 0)
                    {
                        binarizedWeights[rowIndex][block] |= ((uint64_t)1 << bit);
                    }
                }
            }

            std::vector<uint64_t> temp;
            archiver["binarizedWeights_numVectors"] << binarizedWeights.size();
            for (size_t i = 0; i < binarizedWeights.size(); ++i)
            {
                temp.insert(temp.end(), binarizedWeights[i].begin(), binarizedWeights[i].end());
            }
            archiver["binarizedWeights_values"] << temp;
            temp.clear();
            archiver["filterMeans"] << _filterMeans;
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            archiver["receptiveField"] >> _convolutionalParameters.receptiveField;
            archiver["stride"] >> _convolutionalParameters.stride;
            int method;
            archiver["method"] >> method;
            _convolutionalParameters.method = static_cast<BinaryConvolutionMethod>(method);
            int weightsScale;
            archiver["weightsScale"] >> weightsScale;
            _convolutionalParameters.weightsScale = static_cast<BinaryWeightsScale>(weightsScale);

            size_t numVectors = 0;
            std::vector<uint64_t> temp;
            const size_t binarizedFilterVolumeSize = ((_convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels()) + (_binaryElementSize - 1)) / _binaryElementSize;
            archiver["binarizedWeights_numVectors"] >> numVectors;
            archiver["binarizedWeights_values"] >> temp;
            _binarizedWeights.resize(numVectors);
            for (size_t i = 0; i < _binarizedWeights.size(); ++i)
            {
                _binarizedWeights[i].resize(binarizedFilterVolumeSize, 0);
                for (size_t j = 0; j < binarizedFilterVolumeSize; ++j)
                {
                    _binarizedWeights[i][j] = temp[i * binarizedFilterVolumeSize + j];
                }
            }
            archiver["filterMeans"] >> _filterMeans;

            ComputeRealValuedWeightsMatrix();
            InitializeIOMatrices();
            ComputeShapedInputPaddingMask();
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ComputeRealValuedWeightsMatrix()
        {
            const auto filterWidth = _convolutionalParameters.receptiveField;
            const auto numWeightsColumns = filterWidth * filterWidth * _layerParameters.input.NumChannels();
            const size_t binarizedFilterVolumeSize = (numWeightsColumns - 1) / _binaryElementSize + 1;

            _realValuedWeightsMatrix = { NumOutputChannels(), numWeightsColumns };
            for (size_t rowIndex = 0; rowIndex < _binarizedWeights.size(); ++rowIndex)
            {
                size_t colIndex = 0;
                assert(binarizedFilterVolumeSize == _binarizedWeights[rowIndex].size());
                for (size_t blockIndex = 0; blockIndex < binarizedFilterVolumeSize; blockIndex++)
                {
                    const auto bits = _binarizedWeights[rowIndex][blockIndex];
                    const auto filterMean = _filterMeans[rowIndex];

                    ElementType scale(1.0);
                    if (_convolutionalParameters.weightsScale == BinaryWeightsScale::mean)
                    {
                        scale = filterMean;
                    }

                    for (size_t bitIndex = 0; bitIndex < _binaryElementSize && colIndex < numWeightsColumns; ++bitIndex, ++colIndex)
                    {
                        const auto bitVal = (bits >> bitIndex) & 0x01;
                        _realValuedWeightsMatrix(rowIndex, colIndex) = bitVal == 0 ? -scale : scale;
                    }
                }
            }
        }

        template <typename ElementType>
        void BinaryConvolutionalLayer<ElementType>::ComputeShapedInputPaddingMask()
        {
            const size_t fieldVolumeSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels();
            const size_t outputHeight = NumOutputRowsMinusPadding();
            const size_t outputWidth = NumOutputColumnsMinusPadding();
            const size_t rowMax = outputWidth * outputHeight;

            for (size_t outRow = 0; outRow < rowMax; ++outRow)
            {
                const size_t convolutionalRow = outRow / outputWidth;
                const size_t convolutionalCol = outRow % outputWidth;
                const size_t horizontalStart = (convolutionalCol * _convolutionalParameters.stride);
                const size_t verticalStart = (convolutionalRow * _convolutionalParameters.stride);
                int maskSum = 0;

                for (size_t f = 0; f < fieldVolumeSize; ++f)
                {
                    // Calculate the col, row, and depth values in the convolutional field volume
                    const size_t volCol = (f / _layerParameters.input.NumChannels()) % _convolutionalParameters.receptiveField;
                    const size_t volRow = (f / _layerParameters.input.NumChannels()) / _convolutionalParameters.receptiveField;

                    // Calculate where this fits in relation to the input volume
                    const IntPtrT sourceCol = horizontalStart + volCol;
                    const IntPtrT sourceRow = verticalStart + volRow;

                    const size_t block = f / _binaryElementSize;
                    const size_t bit = f % _binaryElementSize;

                    if (bit == 0)
                    {
                        // Initialize to ones
                        _shapedInputPaddingMask[outRow][block] = std::numeric_limits<uint64_t>::max();
                    }

                    // Set the mask for zero padding, so that the effect of these
                    // on the bitwise operation is removed
                    if (IsInputZeroPadding(sourceRow, sourceCol))
                    {
                        _shapedInputPaddingMask[outRow][block] -= ((uint64_t)1 << bit);
                        maskSum += 1;
                    }
                }
                _shapedInputPaddingMaskSums[outRow] = maskSum;
            }
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
