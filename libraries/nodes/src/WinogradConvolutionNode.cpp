////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WinogradConvolutionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WinogradConvolutionNode.h"
#include "ConstantNode.h"

// dsp
#include "WinogradConvolution.h"

// math
#include "Matrix.h"

namespace ell
{
namespace nodes
{
    //
    // Internal utility routines
    //
    namespace
    {
        int GetOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }

        //
        // Winograd-specific routines
        //

        template <typename ValueType, int tileSize, int filterSize>
        struct WinogradTransform;

        template <typename ValueType>
        struct WinogradTransform<ValueType, 2, 3>
        {
            static constexpr int tileSize = 2;
            static constexpr int filterSize = 3;
            static constexpr int windowSize = tileSize + filterSize - 1;

            static void TransformInputWindow(emitters::IRFunctionEmitter& function, llvm::Value* window, llvm::Value* transformedWindow)
            {
                auto d = function.LocalMatrix(window, windowSize, windowSize);
                auto X = function.LocalMatrix(transformedWindow, windowSize, windowSize);

                // Compute B'dB
                X(0, 0) = ((d(0, 0) - d(2, 0)) - (d(0, 2) - d(2, 2)));
                X(0, 1) = ((d(0, 1) - d(2, 1)) + (d(0, 2) - d(2, 2)));
                X(0, 2) = ((d(0, 2) - d(2, 2)) - (d(0, 1) - d(2, 1)));
                X(0, 3) = ((d(0, 1) - d(2, 1)) - (d(0, 3) - d(2, 3)));
                X(1, 0) = ((d(1, 0) + d(2, 0)) - (d(1, 2) + d(2, 2)));
                X(1, 1) = ((d(1, 1) + d(2, 1)) + (d(1, 2) + d(2, 2)));
                X(1, 2) = ((d(1, 2) + d(2, 2)) - (d(1, 1) + d(2, 1)));
                X(1, 3) = ((d(1, 1) + d(2, 1)) - (d(1, 3) + d(2, 3)));
                X(2, 0) = ((d(2, 0) - d(1, 0)) - (d(2, 2) - d(1, 2)));
                X(2, 1) = ((d(2, 1) - d(1, 1)) + (d(2, 2) - d(1, 2)));
                X(2, 2) = ((d(2, 2) - d(1, 2)) - (d(2, 1) - d(1, 1)));
                X(2, 3) = ((d(2, 1) - d(1, 1)) - (d(2, 3) - d(1, 3)));
                X(3, 0) = ((d(1, 0) - d(3, 0)) - (d(1, 2) - d(3, 2)));
                X(3, 1) = ((d(1, 1) - d(3, 1)) + (d(1, 2) - d(3, 2)));
                X(3, 2) = ((d(1, 2) - d(3, 2)) - (d(1, 1) - d(3, 1)));
                X(3, 3) = ((d(1, 1) - d(3, 1)) - (d(1, 3) - d(3, 3)));
            }

            static void TransformOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutputWindow, llvm::Value* outputTile)
            {
                auto X = function.LocalMatrix(transformedOutputWindow, windowSize, windowSize);
                auto result = function.LocalMatrix(outputTile, tileSize, tileSize);

                // Now Compute result tile At * X * A
                result(0, 0) = ((((X(0, 0) + X(1, 0)) + X(2, 0)) + ((X(0, 1) + X(1, 1)) + X(2, 1))) + ((X(0, 2) + X(1, 2)) + X(2, 2)));
                result(0, 1) = ((((X(0, 1) + X(1, 1)) + X(2, 1)) - ((X(0, 2) + X(1, 2)) + X(2, 2))) - ((X(0, 3) + X(1, 3)) + X(2, 3)));
                result(1, 0) = ((((X(1, 0) - X(2, 0)) - X(3, 0)) + ((X(1, 1) - X(2, 1)) - X(3, 1))) + ((X(1, 2) - X(2, 2)) - X(3, 2)));
                result(1, 1) = ((((X(1, 1) - X(2, 1)) - X(3, 1)) - ((X(1, 2) - X(2, 2)) - X(3, 2))) - ((X(1, 3) - X(2, 3)) - X(3, 3)));
            }
        };

        template <typename ValueType>
        void TransformInputWindow(emitters::IRFunctionEmitter& function, llvm::Value* window, int tileSize, int filterSize, llvm::Value* transformedWindow)
        {
            // out = Bt * window * B
            if (tileSize == 2 && filterSize == 3)
            {
                WinogradTransform<ValueType, 2, 3>::TransformInputWindow(function, window, transformedWindow);
            }
        }

        template <typename ValueType>
        void TransformOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutputWindow, int tileSize, int filterSize, llvm::Value* outputTile)
        {
            // out = At * window * A
            if (tileSize == 2 && filterSize == 3)
            {
                WinogradTransform<ValueType, 2, 3>::TransformOutputTile(function, transformedOutputWindow, outputTile);
            }
        }

        // TODO: create a struct to hold the problem size values (inputColumnStride, numChannels, numTileRows, ...)

        //
        // `GetInputWindow()` copies data from the input into a contiguous input window of size (filterSize+tileSize-1) x (filterSize+tileSize-1).
        // If the tile row or column are passed in as compile-time constants, then partial windows will be correctly
        // copied, otherwise this code assumes the window is fully contained in the input.
        //
        void GetInputWindow(emitters::IRFunctionEmitter& function,
                            llvm::Value* input,
                            emitters::IRLocalScalar tileRow,
                            emitters::IRLocalScalar tileColumn,
                            emitters::IRLocalScalar channel,
                            int inputColumnStride,
                            int numChannels,
                            int outputRows,
                            int outputColumns,
                            int tileSize,
                            int filterSize,
                            llvm::Value* inputWindow)
        {
            // input: inputImageRows x inputImageColumns x numChannels tensor
            // inputWindow: windowSize x windowSize block extracted from the given channel with the given upper-left coordinates
            const int windowSize = tileSize + filterSize - 1;
            int windowRowSize = windowSize;
            int windowColumnSize = windowSize;

            // check for constant tile row on last row
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow)))
            {
                auto constRow = llvm::cast<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow));
                auto constRowVal = static_cast<int>(constRow->getSExtValue());
                auto inputRows = outputRows + filterSize - 1;
                auto windowStart = constRowVal * tileSize;
                windowRowSize = std::min(windowSize, inputRows - windowStart);
            }

            // check for constant tile column on last column
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn)))
            {
                auto constColumn = llvm::cast<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn));
                auto constColumnVal = static_cast<int>(constColumn->getSExtValue());
                auto inputColumns = outputColumns + filterSize - 1;
                auto windowStart = constColumnVal * tileSize;
                windowColumnSize = std::min(windowSize, inputColumns - windowStart);
            }

            // Retrieve values from input into local matrix of size tr x tc
            auto imageRow = tileRow * tileSize;
            auto imageColumn = tileColumn * tileSize;
            auto offset = ((imageRow * inputColumnStride) + imageColumn) * numChannels + channel; // offset to upper-left of input tile
            for (int rowIndex = 0; rowIndex < windowRowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowColumnSize; ++columnIndex)
                {
                    auto inputLoc = offset + ((rowIndex * inputColumnStride) + columnIndex) * numChannels;
                    auto outputLoc = rowIndex * windowSize + columnIndex;
                    auto value = function.ValueAt(input, inputLoc);
                    function.SetValueAt(inputWindow, outputLoc, value);
                }
            }

            // Zero out unused parts
            for (int rowIndex = windowRowSize; rowIndex < windowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowSize; ++columnIndex)
                {
                    auto outputLoc = rowIndex * windowSize + columnIndex;
                    function.StoreZero(function.PointerOffset(inputWindow, outputLoc));
                }
            }
            for (int rowIndex = 0; rowIndex < windowRowSize; ++rowIndex)
            {
                for (int columnIndex = windowColumnSize; columnIndex < windowSize; ++columnIndex)
                {
                    auto outputLoc = rowIndex * windowSize + columnIndex;
                    function.StoreZero(function.PointerOffset(inputWindow, outputLoc));
                }
            }
        }

        template <typename ValueType>
        void GetTransformedInputWindow(emitters::IRFunctionEmitter& function,
                                       llvm::Value* input,
                                       emitters::IRLocalScalar tileRow,
                                       emitters::IRLocalScalar tileColumn,
                                       emitters::IRLocalScalar channel,
                                       int inputColumnStride,
                                       int numChannels,
                                       int outputRows,
                                       int outputColumns,
                                       int tileSize,
                                       int filterSize,
                                       llvm::Value* transformedInput)
        {
            // input: inputImageRows x inputImageColumns x numChannels tensor
            // transformedInput is a (wr*wc) x numChannels x (tr * tc) tensor containing the entire transformed input signal
            const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
            const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;
            const int windowSize = tileSize + filterSize - 1;

            llvm::AllocaInst* inputWindow = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize);
            GetInputWindow(function, input, tileRow, tileColumn, channel, inputColumnStride, numChannels, outputRows, outputColumns, tileSize, filterSize, inputWindow);

            llvm::AllocaInst* transformedWindow = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize);
            TransformInputWindow<ValueType>(function, inputWindow, tileSize, filterSize, transformedWindow);

            // transformedInput is a (wr*wc) x numChannels x (tr * tc) tensor containing the entire transformed input signal.
            // Think of it as (wr*wc) separate numChannels x tr x tc tensors: one tr x tc image for each position in the transformed window.
            // So, there's a tensor representing the upper-left window pixel for each transformed input tile, another representing the (0,1) pixel
            // of each tranformed input tile, and so on.
            // This operation is dispersing the computed values in a maximally discontiguous way. We can likely speed it up by processing
            // contiguous entries in the output tensors. That is, processing multiple windows at a time. Processing multiple input windows at a time
            // will also enable reusing the overlapped input window data across tiles.
            auto offset = ((((channel * numPartialTileRows) + tileRow) * numPartialTileColumns) + tileColumn); // this is the offset within a (numChannels x tr x tc) tensor for the input window we're processing
            auto stride = numChannels * numPartialTileRows * numPartialTileColumns;

            for (int rowIndex = 0; rowIndex < windowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowSize; ++columnIndex)
                {
                    auto windowLoc = (rowIndex * windowSize) + columnIndex;
                    auto outputLoc = windowLoc * stride + offset;
                    auto value = function.ValueAt(transformedWindow, windowLoc);
                    function.SetValueAt(transformedInput, outputLoc, value);
                }
            }
        }

        template <typename ValueType>
        void GetTransformedOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutput, emitters::IRLocalScalar tileRow, emitters::IRLocalScalar tileColumn, emitters::IRLocalScalar filterIndex, int outputRows, int outputColumns, int numFilters, int tileSize, int filterSize, llvm::Value* outputTile)
        {
            const int windowSize = tileSize + filterSize - 1;
            const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
            const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;

            llvm::AllocaInst* transformedOutputWindow = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize);

            // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
            // transformedOutput is (wr*wc) x nf x tr x tc, where tr == # tile rows and tc == # tile columns
            // transformedOutputWindow is (wr*wc)

            // Retrieve values from transformed output into local matrix of size wr x wc
            auto stride = numFilters * numPartialTileRows * numPartialTileColumns;
            auto offset = (filterIndex * numPartialTileRows * numPartialTileColumns) + (tileRow * numPartialTileColumns) + tileColumn;

            // rowIndex and columnIndex are the row and column indices within the window
            for (int rowIndex = 0; rowIndex < windowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowSize; ++columnIndex)
                {
                    auto windowLoc = (rowIndex * windowSize) + columnIndex;
                    auto inputLoc = windowLoc * stride + offset;
                    auto value = function.ValueAt(transformedOutput, inputLoc);
                    function.SetValueAt(transformedOutputWindow, windowLoc, value);
                }
            }

            TransformOutputTile<ValueType>(function, transformedOutputWindow, tileSize, filterSize, outputTile);
        }

        //
        // `SplatTransformedOutputTile()` copies data for a transformed output tile into the correct place in the output
        // If the tile row or column are passed in as compile-time constants, then partial tiles will be correctly
        // copied, otherwise this code assumes the tile is fully contained in the output.
        //
        template <typename ValueType>
        void SplatTransformedOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutput, emitters::IRLocalScalar tileRow, emitters::IRLocalScalar tileColumn, emitters::IRLocalScalar filterIndex, int outputRows, int outputColumns, int numFilters, int tileSize, int filterSize, llvm::Value* output)
        {
            auto tileRowSize = tileSize;
            auto tileColumnSize = tileSize;

            // outputTile is tile (tr, tc, f) of the output
            // output is a r, c, nf image tensor containing the convolution result
            llvm::AllocaInst* outputTile = function.Variable(emitters::GetVariableType<ValueType>(), tileSize * tileSize);
            GetTransformedOutputTile<ValueType>(function, transformedOutput, tileRow, tileColumn, filterIndex, outputRows, outputColumns, numFilters, tileSize, filterSize, outputTile);

            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow)))
            {
                auto constRow = llvm::cast<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow));
                auto constRowVal = static_cast<int>(constRow->getSExtValue());
                auto tileStart = constRowVal * tileSize;
                tileRowSize = std::min(tileSize, outputRows - tileStart);
            }

            // check for constant tile column on last column
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn)))
            {
                auto constColumn = llvm::cast<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn));
                auto constColumnVal = static_cast<int>(constColumn->getSExtValue());
                auto tileStart = constColumnVal * tileSize;
                tileColumnSize = std::min(tileSize, outputColumns - tileStart);
            }

            // rowIndex and columnIndex are the row and column indices within the tile
            auto offset = ((tileSize * tileRow * outputColumns) + (tileSize * tileColumn)) * numFilters + filterIndex; // offset into the upper-left of the given tile
            for (int rowIndex = 0; rowIndex < tileRowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < tileColumnSize; ++columnIndex)
                {
                    auto inputLoc = (rowIndex * tileSize) + columnIndex;
                    auto outputLoc = ((rowIndex * outputColumns) + columnIndex) * numFilters + offset; // This is incorrect
                    auto value = function.ValueAt(outputTile, inputLoc);
                    function.SetValueAt(output, outputLoc, value);
                }
            }
        }

        //
        // Core algorithm parts
        //
        template <typename ValueType>
        void TransformInput(emitters::IRFunctionEmitter& function,
                            llvm::Value* input,
                            int outputRows,
                            int outputColumns,
                            int numChannels,
                            int inputColumnStride,
                            int tileSize,
                            int filterSize,
                            llvm::Value* transformedInput)
        {
            const auto numTileRows = outputRows / tileSize;
            const auto numTileColumns = outputColumns / tileSize;
            const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
            const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;

            // Get transformed input for all "full" windows (ones that don't fall off the edge or bottom of the input image)
            function.For(numTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                auto rowTileIndex = function.LocalScalar(index1);
                function.For(numTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto columnTileIndex = function.LocalScalar(index2);
                    function.For(numChannels, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto channelIndex = function.LocalScalar(index3);
                        GetTransformedInputWindow<ValueType>(function, input, rowTileIndex, columnTileIndex, channelIndex, inputColumnStride, numChannels, outputRows, outputColumns, tileSize, filterSize, transformedInput);
                    });
                });
            });

            // Extra rows on the bottom
            if (numPartialTileRows > numTileRows)
            {
                auto rowTileIndex = function.LocalScalar(numTileRows);
                function.For(numTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto columnTileIndex = function.LocalScalar(index2);
                    function.For(numChannels, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto channelIndex = function.LocalScalar(index3);
                        GetTransformedInputWindow<ValueType>(function, input, rowTileIndex, columnTileIndex, channelIndex, inputColumnStride, numChannels, outputRows, outputColumns, tileSize, filterSize, transformedInput);
                    });
                });
            }

            // Extra columns on the right
            if (numPartialTileColumns > numTileColumns)
            {
                auto columnTileIndex = function.LocalScalar(numTileColumns);
                function.For(numTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                    auto rowTileIndex = function.LocalScalar(index1);
                    function.For(numChannels, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto channelIndex = function.LocalScalar(index3);
                        GetTransformedInputWindow<ValueType>(function, input, rowTileIndex, columnTileIndex, channelIndex, inputColumnStride, numChannels, outputRows, outputColumns, tileSize, filterSize, transformedInput);
                    });
                });
            }

            // Extra entries in the bottom-right corner
            if ((numPartialTileRows > numTileRows) && (numPartialTileColumns > numTileColumns))
            {
                auto rowTileIndex = function.LocalScalar(numTileRows);
                auto columnTileIndex = function.LocalScalar(numTileColumns);
                function.For(numChannels, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                    auto channelIndex = function.LocalScalar(index3);
                    GetTransformedInputWindow<ValueType>(function, input, rowTileIndex, columnTileIndex, channelIndex, inputColumnStride, numChannels, outputRows, outputColumns, tileSize, filterSize, transformedInput);
                });
            }
        }

        // Apply the (tranformed) filters to the transformed input to produce the transformed output
        template <typename ValueType>
        void ComputeTransformedOutput(emitters::IRFunctionEmitter& function,
                                      llvm::Value* transformedInput,
                                      llvm::Value* transformedFilters,
                                      int outputRows,
                                      int outputColumns,
                                      int numChannels,
                                      int numFilters,
                                      int tileSize,
                                      int filterSize,
                                      llvm::Value* transformedOutput)
        {
            const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
            const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;

            int windowSize = filterSize + tileSize - 1;

            // Now, transformedInput is a tensor of dimensions (wsxwsx?x?), which we can multiply by the stored filter matrix
            // Now do the multiply to reduce many entries in parallel
            //
            // transformedInput is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
            // transformedFilters is a (wr*wc) x nf x d tensor
            // transformedOutput is (wr*wc) x nf x (tr * tc)
            // transformedFilters(i,j) * transformedInput(i,j) -> transformedOutput  (nf x d) * (d x (tr*tc)) -> nf x (tr*tc)

            // These strides are the distance between spatially-adjacent window entries in the various data structures
            int numOutputTiles = numPartialTileRows * numPartialTileColumns;
            int transformedInputStride = numChannels * numOutputTiles;
            int transformedFiltersStride = numFilters * numChannels;
            int transformedOutputStride = numFilters * numOutputTiles;

            // Each window pixel position has a separate matrix of values to transform via a matrix multiply
            // rowIndex, columnIndex are the row and column indicies within the window
            for (int rowIndex = 0; rowIndex < windowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowSize; ++columnIndex)
                {
                    // Compute the offsets to the particular (wr, wc) matrix we want
                    const auto windowIndex = (rowIndex * windowSize) + columnIndex;
                    auto transformedInputMatrix = function.PointerOffset(transformedInput, windowIndex * transformedInputStride);
                    auto transformedFiltersMatrix = function.PointerOffset(transformedFilters, windowIndex * transformedFiltersStride);
                    auto transformedOutputMatrix = function.PointerOffset(transformedOutput, windowIndex * transformedOutputStride); // wrong location in memory

                    // filter: m x k, input: k x n, output: m x n
                    // transformedOutput = transformedFilter * transformedInput
                    int m = numFilters;
                    int n = numOutputTiles;
                    int k = numChannels;

                    // Now do a matrix multiply to reduce many entries in parallel
                    function.CallGEMM<ValueType>(false, false, m, n, k, transformedFiltersMatrix, k, transformedInputMatrix, n, transformedOutputMatrix, n);
                }
            }
        }

        template <typename ValueType>
        void TransformOutput(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutput, int outputRows, int outputColumns, int numFilters, int tileSize, int filterSize, llvm::Value* output)
        {
            const auto numTileRows = outputRows / tileSize;
            const auto numTileColumns = outputColumns / tileSize;
            const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
            const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;

            // First output all of the complete tiles
            function.For(numFilters, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                auto filterIndex = function.LocalScalar(index1);
                function.For(numTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto rowTileIndex = function.LocalScalar(index2);
                    function.For(numTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto columnTileIndex = function.LocalScalar(index3);

                        SplatTransformedOutputTile<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, filterIndex, outputRows, outputColumns, numFilters, tileSize, filterSize, output);
                    });
                });
            });

            // Now, if there are any partially-filled tiles on the right and left, output them:

            // Extra rows on the bottom
            if (numPartialTileRows > numTileRows)
            {
                auto rowTileIndex = function.LocalScalar(numTileRows);
                function.For(numFilters, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                    auto filterIndex = function.LocalScalar(index1);
                    function.For(numTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto columnTileIndex = function.LocalScalar(index3);
                        SplatTransformedOutputTile<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, filterIndex, outputRows, outputColumns, numFilters, tileSize, filterSize, output);
                    });
                });
            }

            // Extra columns on the right
            if (numPartialTileColumns > numTileColumns)
            {
                auto columnTileIndex = function.LocalScalar(numTileColumns);
                function.For(numFilters, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                    auto filterIndex = function.LocalScalar(index1);
                    function.For(numTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                        auto rowTileIndex = function.LocalScalar(index2);
                        SplatTransformedOutputTile<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, filterIndex, outputRows, outputColumns, numFilters, tileSize, filterSize, output);
                    });
                });
            }

            // Extra entries in the lower-right partial tile
            if ((numPartialTileRows > numTileRows) && (numPartialTileColumns > numTileColumns))
            {
                auto rowTileIndex = function.LocalScalar(numTileRows);
                auto columnTileIndex = function.LocalScalar(numTileColumns);
                function.For(numFilters, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                    auto filterIndex = function.LocalScalar(index1);
                    SplatTransformedOutputTile<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, filterIndex, outputRows, outputColumns, numFilters, tileSize, filterSize, output);
                });
            }
        }
    } // end anonymous namespace

    //
    // WinogradConvolutionNode
    //

    template <typename ValueType>
    WinogradConvolutionNode<ValueType>::WinogradConvolutionNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    WinogradConvolutionNode<ValueType>::WinogradConvolutionNode(const WinogradConvolutionNode<ValueType>& other, const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(other._outputMemoryLayout)), _inputMemoryLayout(other._inputMemoryLayout), _outputMemoryLayout(other._outputMemoryLayout), _filterWeights(other._filterWeights), _stride(other._stride), _tileSize(other._tileSize), _filterSize(other._filterSize)
    {
    }

    template <typename ValueType>
    WinogradConvolutionNode<ValueType>::WinogradConvolutionNode(const model::PortElements<ValueType>& input,
                                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                                const ConstTensorReferenceType& filterWeights,
                                                                size_t stride)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _stride(static_cast<int>(stride))
    {
        const int numFilters = outputMemoryLayout.GetActiveSize(2);
        // filters is (nf*fr) x fc x d
        _filterSize = filterWeights.NumColumns();
        assert(filterWeights.NumRows() == _filterSize * numFilters);
        _tileSize = 2;
        _filterWeights = dsp::GetTransformedFilters(filterWeights, numFilters, _tileSize, dsp::WinogradAlgorithmVersion::v2);
    }

    template <typename ValueType>
    void WinogradConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<WinogradConvolutionNode<ValueType>>(*this, newInput);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    bool WinogradConvolutionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        const auto& weightsMatrix = _filterWeights.ReferenceAsMatrix();
        const auto weightsValues = weightsMatrix.ToArray();

        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto convNode = transformer.AddNode<WinogradConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, _outputMemoryLayout, _stride, _tileSize, _filterSize);
        transformer.MapNodeOutput(this->output, convNode->output);
        return true;
    }

    template <typename ValueType>
    void WinogradConvolutionNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void WinogradConvolutionNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << _outputMemoryLayout;
        archiver["tileSize"] << _tileSize;
        archiver["filterSize"] << _filterSize;
        archiver["stride"] << _stride;
        math::TensorArchiver::Write(_filterWeights, "weights", archiver);
    }

    template <typename ValueType>
    void WinogradConvolutionNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        archiver["outputLayout"] >> _outputMemoryLayout;
        archiver["tileSize"] >> _tileSize;
        archiver["filterSize"] >> _filterSize;
        archiver["stride"] >> _stride;
        math::TensorArchiver::Read(_filterWeights, "weights", archiver);
    }

    //
    // WinogradConvolutionComputeNode
    //

    template <typename ValueType>
    WinogradConvolutionComputeNode<ValueType>::WinogradConvolutionComputeNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _filterWeights(this, {}, filterWeightsPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    WinogradConvolutionComputeNode<ValueType>::WinogradConvolutionComputeNode(const model::PortElements<ValueType>& input,
                                                                              const model::PortElements<ValueType>& filterWeights,
                                                                              const model::PortMemoryLayout& inputMemoryLayout,
                                                                              const model::PortMemoryLayout& outputMemoryLayout,
                                                                              int stride,
                                                                              int tileSize,
                                                                              int filterSize)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _stride(stride), _tileSize(tileSize), _filterSize(filterSize)
    {
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<WinogradConvolutionComputeNode<ValueType>>(newInput, newFilterWeights, _inputMemoryLayout, _outputMemoryLayout, _stride, _tileSize, _filterSize);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    // Terminology:
    // fw: filter width
    // d: # input channels
    // f: # filters (== output channels)

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        using namespace std::string_literals;

        auto& module = function.GetModule();

        // input is a d x (w+2p) x (h+2p) array
        // reshaped, it's a d*(w+2p)) x (h+2p) array == d*(w+k-1) x (h+k-1)
        llvm::Value* input = compiler.EnsurePortEmitted(this->input);

        // weights is f x k x k x d array
        // reshaped, it's (f*k) x (k*d) or f x k x (k*d)
        // transformedFilters is a (wr*wc) x nf x d tensor
        llvm::Value* transformedFilters = compiler.EnsurePortEmitted(this->filterWeights);

        // output is a (w+2p) x (h+2p) x f array
        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

        // Input data parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto inputRowStride = inputLayout.GetStride(1);
        const auto inputColumnStride = inputLayout.GetStride(1);
        const auto numChannels = inputLayout.GetActiveSize(2);
        UNUSED(inputRowStride);

        // Output data parameters
        const auto outputLayout = this->GetOutputMemoryLayout();
        const int outputRows = outputLayout.GetActiveSize(0);
        const int outputColumns = outputLayout.GetActiveSize(1);
        const int numFilters = outputLayout.GetActiveSize(2);

        // Filter parameters
        const int inputPadding = inputLayout.GetOffset(0);
        DEBUG_USED(inputPadding);
        assert((inputPadding == _filterSize / 2) && "Padding must be filterSize/2");

        // Winograd-specific stuff
        const auto tileSize = _tileSize;
        const auto filterSize = _filterSize;
        const auto windowSize = _filterSize + tileSize - 1;
        const auto numPartialTileRows = (outputRows + tileSize - 1) / tileSize;
        const auto numPartialTileColumns = (outputColumns + tileSize - 1) / tileSize;

        // Allocate scratch space --- rounding up the number of tiles
        int transformedInputSize = windowSize * windowSize * numChannels * numPartialTileRows * numPartialTileColumns;
        int transformedOutputSize = windowSize * windowSize * numFilters * numPartialTileRows * numPartialTileColumns;
        auto transformedInput = module.GlobalArray<ValueType>("transformedInput"s + GetInternalStateIdentifier(), transformedInputSize);
        auto transformedOutput = module.GlobalArray<ValueType>("transformedOutput"s + GetInternalStateIdentifier(), transformedOutputSize);

        // transformedInput is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
        // transformedFilters is a (wr*wc) x nf x d tensor
        // transformedOutput is (wr*wc) x nf x (tr * tc)
        // transformedFilters(i,j) * transformedInput(i,j) -> transformedOutput  (nf x d) * (d x (tr*tc)) -> nf x (tr*tc)

        // This is the core of the Winograd convolution algorithm: transform the input, perform an elementwise multiply between it an the transformed filter, and transform it back
        TransformInput<ValueType>(function, input, outputRows, outputColumns, numChannels, inputColumnStride, tileSize, filterSize, transformedInput);
        ComputeTransformedOutput<ValueType>(function, transformedInput, transformedFilters, outputRows, outputColumns, numChannels, numFilters, tileSize, filterSize, transformedOutput);
        TransformOutput<ValueType>(function, transformedOutput, outputRows, outputColumns, numFilters, tileSize, filterSize, output);
    }

    // Explicit specializations
    template class WinogradConvolutionNode<float>;
    template class WinogradConvolutionNode<double>;
} // nodes
} // ell
