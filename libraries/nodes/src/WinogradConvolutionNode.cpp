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
        #if 0
        //
        // Useful debugging utilities
        //
        template <typename ValueType>
        void PrintBlock(emitters::IRFunctionEmitter& function,
                        llvm::Value* block,
                        int rows, int columns, int channels)
        {
            auto blockArray = function.LocalMultidimArray(block, { rows, columns, channels });
            for (int rowIndex = 0; rowIndex < rows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
                {
                    for (int channelIndex = 0; channelIndex < channels; ++channelIndex)
                    {
                        emitters::IRLocalScalar val = blockArray({ rowIndex, columnIndex, channelIndex });
                        function.Printf("%f ", { function.CastValue<ValueType, double>(val) });
                    }
                    function.Print("  ");
                }
                function.Print("\n");
            }
        }
        #endif

        //
        // Misc
        //
        std::string to_string(dsp::WinogradFilterOrder order)
        {
            switch (order)
            {
            case dsp::WinogradFilterOrder::filtersFirst:
                return "filtersFirst";
                break;
            case dsp::WinogradFilterOrder::tilesFirst:
                return "tilesFirst";
                break;
            default:
                throw utilities::DataFormatException(utilities::DataFormatErrors::illegalValue);
            }
        }

        dsp::WinogradFilterOrder filter_order_from_string(std::string name)
        {
            if (name == "filtersFirst")
            {
                return dsp::WinogradFilterOrder::filtersFirst;
            }
            else if (name == "tilesFirst")
            {
                return dsp::WinogradFilterOrder::tilesFirst;
            }
            else
            {
                throw utilities::DataFormatException(utilities::DataFormatErrors::illegalValue);
            }
        }

        int GetOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }

        emitters::IRLocalScalar AddAndSimplify(emitters::IRLocalScalar a, emitters::IRLocalScalar b)
        {
            if (auto constA = llvm::dyn_cast<llvm::ConstantFP>(a.value))
            {
                if (constA->isZero())
                {
                    return b;
                }
                if (auto constB = llvm::dyn_cast<llvm::ConstantFP>(b.value))
                {
                    // a and b both constants
                    const auto& aValue = constA->getValueAPF();
                    const auto& bValue = constB->getValueAPF();
                    return a.function.LocalScalar(llvm::ConstantFP::get(a.value->getContext(), aValue + bValue));
                }
            }
            if (auto constB = llvm::dyn_cast<llvm::ConstantFP>(b.value))
            {
                if (constB->isZero())
                {
                    return a;
                }
            }
            return a + b;
        }

        emitters::IRLocalScalar MultiplyAndSimplify(emitters::IRLocalScalar a, emitters::IRLocalScalar b)
        {
            auto& function = a.function;
            if (auto constA = llvm::dyn_cast<llvm::ConstantFP>(a.value))
            {
                if (auto constB = llvm::dyn_cast<llvm::ConstantFP>(b.value))
                {
                    // a and b both constants
                    const auto& aValue = constA->getValueAPF();
                    const auto& bValue = constB->getValueAPF();
                    return function.LocalScalar(llvm::ConstantFP::get(a.value->getContext(), aValue * bValue));
                }
                if (constA->isZero())
                {
                    return function.LocalScalar(llvm::ConstantFP::get(a.value->getType(), 0.0));
                }
                if (constA->isExactlyValue(1.0))
                {
                    return b;
                }
            }
            if (auto constB = llvm::dyn_cast<llvm::ConstantFP>(b.value))
            {
                if (constB->isZero())
                {
                    return function.LocalScalar(llvm::ConstantFP::get(a.value->getType(), 0.0));
                }
                if (constB->isExactlyValue(1.0))
                {
                    return a;
                }
            }

            return a * b;
        }

        // computes (a*b) + c, unless c is invalid, in which case return (a*b)
        emitters::IRLocalScalar MultiplyAddSafe(emitters::IRLocalScalar a, emitters::IRLocalScalar b, emitters::IRLocalScalar c)
        {
            auto product = MultiplyAndSimplify(a, b);
            return c.IsValid() ? AddAndSimplify(product, c) : product;
        }

        template <typename ValueType>
        int GetConstantIntValue(llvm::Value* value)
        {
            static_assert(std::is_integral<ValueType>(), "Template parameter must be integral");
            assert(llvm::isa<llvm::ConstantInt>(value));
            auto constIntVal = llvm::cast<llvm::ConstantInt>(value);
            return static_cast<ValueType>(constIntVal->getSExtValue());
        }

        //
        // IRLocalValueMatrix is just a class that holds a rectangular array of LLVM IR values
        // so that we can do matrix algebra with them easily. The matrix doesn't correspond to
        // a region of memory on the target device, the elements are just expressions built up
        // at compile time.
        //
        class IRLocalValueMatrix
        {
        public:
            IRLocalValueMatrix(emitters::IRFunctionEmitter& function, int rows, int columns)
                : function(function), rows(rows), columns(columns)
            {
                for (int index = 0; index < (rows * columns); ++index)
                {
                    _data.push_back(function.LocalScalar());
                }
            }

            emitters::IRLocalScalar operator()(int row, int column) const
            {
                return _data[GetIndex(row, column)];
            }

            emitters::IRLocalScalar& operator()(int row, int column)
            {
                return _data[GetIndex(row, column)];
            }

            // properties
            emitters::IRFunctionEmitter& function;
            int rows = 0;
            int columns = 0;

        private:
            std::vector<emitters::IRLocalScalar> _data;
            int GetIndex(int row, int column) const
            {
                return row * columns + column;
            }
        };

        //
        // IRLocalValueMultidimArray is just a class that holds a multidimensional array of LLVM IR values.
        // The array doesn't correspond to a region of memory on the target device, the elements are just
        // expressions built up at compile time.
        //
        class IRLocalValueMultidimArray
        {
        public:
            IRLocalValueMultidimArray(emitters::IRFunctionEmitter& function, std::initializer_list<int> dimensions)
                : function(function)
            {
                strides.reserve(dimensions.size());
                std::copy(dimensions.begin() + 1, dimensions.end(), std::back_inserter(strides));
                strides.push_back(1);
                int currentStride = 1;
                int numEntries = 1;
                for (auto d : dimensions)
                {
                    numEntries *= d;
                }

                for (auto it = std::rbegin(strides); it != std::rend(strides); ++it)
                {
                    *it *= currentStride;
                    currentStride = *it;
                }

                for (int index = 0; index < numEntries; ++index)
                {
                    _data.push_back(function.LocalScalar());
                }
            }

            emitters::IRLocalScalar operator()(std::initializer_list<int> indices) const
            {
                return _data[GetIndex(indices)];
            }

            emitters::IRLocalScalar& operator()(std::initializer_list<int> indices)
            {
                return _data[GetIndex(indices)];
            }

            // properties
            emitters::IRFunctionEmitter& function;
            std::vector<int> dimensions;
            std::vector<int> strides;

        private:
            int GetIndex(std::initializer_list<int> indices) const
            {
                assert(indices.size() == strides.size());
                int offset = 0;
                auto stridesIt = strides.begin();
                for (auto& i : indices)
                {
                    offset += i * (*stridesIt);
                    ++stridesIt;
                }
                return offset;
            }

            std::vector<emitters::IRLocalScalar> _data;
        };

        template <typename ValueType>
        IRLocalValueMatrix GetLocalMatrix(emitters::IRFunctionEmitter& function, const math::RowMatrix<ValueType>& m)
        {
            auto numRows = static_cast<int>(m.NumRows());
            auto numColumns = static_cast<int>(m.NumColumns());
            auto result = IRLocalValueMatrix(function, numRows, numColumns);
            for (int i = 0; i < numRows; ++i)
            {
                for (int j = 0; j < numColumns; ++j)
                {
                    result(i, j) = function.LocalScalar<ValueType>(m(i, j));
                }
            }
            return result;
        }

        // Compute a * b * a', for each channel in a tensor b
        template <typename IRMatrixType1, typename IRTensorType2>
        IRLocalValueMultidimArray MatrixMatrixTransposeMultiply(const IRMatrixType1& a, const IRTensorType2& b, int blockSize)
        {
            // a: m x k,  b: k x k x blockSize,  result: m x m x blockSize
            auto m = a.rows;
            auto k = a.columns;

            auto result = IRLocalValueMultidimArray(a.function, { m, m, blockSize });
            for (int channel = 0; channel < blockSize; ++channel)
            {
                for (int ii = 0; ii < m; ++ii)
                {
                    for (int jj = 0; jj < m; ++jj)
                    {
                        auto sum = a.function.LocalScalar();
                        for (int kk = 0; kk < k; ++kk)
                        {
                            for (int ll = 0; ll < k; ++ll)
                            {
                                sum = MultiplyAddSafe(MultiplyAndSimplify(a(ii, ll), b({ ll, kk, channel })), a(jj, kk), sum);
                            }
                        }
                        result({ ii, jj, channel }) = sum;
                    }
                }
            }
            return result;
        }

        // Compute a * b * a', for each channel in b
        template <typename IRMatrixType1, typename IRTensorType2>
        void MatrixMatrixTransposeMultiplyInto(const IRMatrixType1& a, const IRTensorType2& b, int blockSize, emitters::IRLocalMultidimArray& c)
        {
            // a: m x k,  b: k x k x blockSize,  c: m x m x blockSize
            auto m = a.rows;
            auto k = a.columns;
            auto& function = a.function;

            // The size of the input blocks is generally larger than the blocks we want to operate on when
            // doing the transformation itself. So, we'll break the block up into sub-blocks
            function.For(blockSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto channelIndex = function.LocalScalar(i);

                // Compute a * b * a' for a sub-block of b
                auto inputSubBlock = function.PointerOffset(b.data, channelIndex);
                auto d = function.LocalTensor(inputSubBlock, { k, k, blockSize }, emitters::RowMajorTensorLayout);

                auto Xvals = MatrixMatrixTransposeMultiply(a, d, 1);

                for (int i = 0; i < m; ++i)
                {
                    for (int j = 0; j < m; ++j)
                    {
                        c({ function.LocalScalar(i), function.LocalScalar(j), channelIndex }) = Xvals({ i, j, 0 });
                    }
                }
            });
        }

        void ElementwiseMultiply(emitters::IRFunctionEmitter& function, llvm::Value* AMem, llvm::Value* BMem, int numEntries, llvm::Value* CMem)
        {
            auto A = function.LocalArray(AMem);
            auto B = function.LocalArray(BMem);
            auto C = function.LocalArray(CMem);
            function.For(numEntries, [A, B, C](emitters::IRFunctionEmitter& function, auto i) {
                C[i] = A[i] * B[i];
            });
        }

        //
        // Winograd-specific routines
        //
        template <typename ValueType>
        void TransformInputBlock(emitters::IRFunctionEmitter& function, llvm::Value* inputBlock, int tileSize, int filterSize, int blockSize, llvm::Value* transformedInputBlock)
        {
            int windowSize = tileSize + filterSize - 1;
            auto Bt = GetLocalMatrix(function, dsp::GetLeftDataTransformMatrix<ValueType>(tileSize, filterSize));

            // Compute X = B'dB
            auto d = function.LocalMultidimArray(inputBlock, { windowSize, windowSize, blockSize });
            auto outputBlock = function.LocalMultidimArray(transformedInputBlock, { windowSize, windowSize, blockSize });
            MatrixMatrixTransposeMultiplyInto(Bt, d, blockSize, outputBlock);
        }

        template <typename ValueType>
        void TransformOutputBlock(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutputBlock, int tileSize, int filterSize, int blockSize, llvm::Value* outputBlock)
        {
            int windowSize = tileSize + filterSize - 1;
            auto At = GetLocalMatrix(function, dsp::GetLeftResultTransformMatrix<ValueType>(tileSize, filterSize));

            // Compute result tile At * X * A
            auto X = function.LocalMultidimArray(transformedOutputBlock, { windowSize, windowSize, blockSize });
            auto result = function.LocalMultidimArray(outputBlock, { tileSize, tileSize, blockSize });
            MatrixMatrixTransposeMultiplyInto(At, X, blockSize, result);
        }

        //
        // `GetInputBlock()` copies data from the input into a contiguous input window of size (filterSize+tileSize-1) x (filterSize+tileSize-1).
        // If the tile row or column are passed in as compile-time constants, then partial windows will be correctly
        // copied, otherwise this code assumes the window is fully contained in the input.
        //
        template <typename ValueType>
        void GetInputBlock(emitters::IRFunctionEmitter& function,
                           llvm::Value* input,
                           emitters::IRLocalScalar tileRow,
                           emitters::IRLocalScalar tileColumn,
                           emitters::IRLocalScalar channel,
                           int inputRowStride,
                           int numChannels,
                           int numOutputRows,
                           int numOutputColumns,
                           int tileSize,
                           int filterSize,
                           int blockSize,
                           llvm::Value* inputBlock)
        {
            // input: inputImageRows x inputImageColumns x numChannels tensor
            // inputBlock: windowSize x windowSize x blockSize block extracted from the given channel with the given upper-left coordinates
            const int windowSize = tileSize + filterSize - 1;
            int windowRows = windowSize;
            int windowColumns = windowSize;
            const auto inputRows = numOutputRows + filterSize - 1;
            const auto inputColumns = numOutputColumns + filterSize - 1;
            const auto inputColumnStride = numChannels;

            // check for constant tile row on last row
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow)))
            {
                auto tileRowStart = GetConstantIntValue<int>(tileRow) * tileSize;
                windowRows = std::min(windowSize, inputRows - tileRowStart);
            }

            // check for constant tile column on last column
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn)))
            {
                auto tileColumnStart = GetConstantIntValue<int>(tileColumn) * tileSize;
                windowColumns = std::min(windowSize, inputColumns - tileColumnStart);
            }

            // Retrieve values from input into local matrix of size windowSize x windowSize x blockSize
            auto imageRow = tileRow * tileSize;
            auto imageColumn = tileColumn * tileSize;
            auto inputWindowOffset = (imageRow * inputRowStride) + (imageColumn * inputColumnStride) + channel; // offset to given channel of the upper-left of input tile
            for (int rowIndex = 0; rowIndex < windowRows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < windowColumns; ++columnIndex)
                {
                    auto inputLoc = (rowIndex * inputRowStride) + (columnIndex * inputColumnStride) + inputWindowOffset;
                    auto windowLoc = (rowIndex * windowSize + columnIndex) * blockSize;
                    function.MemoryCopy<ValueType>(input, inputLoc, inputBlock, function.Literal<int>(windowLoc), function.Literal<int>(blockSize));
                }
            }

            // Zero out unused parts
            // First, the righthand edge:
            if (windowSize - windowColumns > 0)
            {
                for (int rowIndex = 0; rowIndex < windowRows; ++rowIndex)
                {
                    auto outputLoc = (rowIndex * windowSize + windowColumns) * blockSize;
                    function.StoreZero(function.PointerOffset(inputBlock, outputLoc), (windowSize - windowColumns) * blockSize);
                }
            }

            // Then the bottom part:
            if (windowSize - windowRows > 0)
            {
                auto outputLoc = windowRows * windowSize * blockSize;
                function.StoreZero(function.PointerOffset(inputBlock, outputLoc), (windowSize - windowRows) * windowSize * blockSize);
            }
        }

        // transformedInput is a windowSize x windowSize x tileRows x tileColumns x numChannels  tensor containing the entire transformed input signal.
        // Think of it as (windowSize*windowSize) separate tileRows x tileColumns x numChannels tensors: one tileRows x tileColumns image for each position in the transformed window.
        // So, there's a tensor representing the upper-left window pixel for each transformed input tile, another representing the (0,1) pixel
        // of each tranformed input tile, and so on.
        template <typename ValueType>
        void SplatTransformedInputBlock(emitters::IRFunctionEmitter& function,
                                        llvm::Value* transformedInputBlock,
                                        emitters::IRLocalScalar tileRow,
                                        emitters::IRLocalScalar tileColumn,
                                        emitters::IRLocalScalar channel,
                                        int numOutputRows,
                                        int numOutputColumns,
                                        int numChannels,
                                        int tileSize,
                                        int filterSize,
                                        int blockSize,
                                        llvm::Value* transformedInput)
        {
            const int windowSize = tileSize + filterSize - 1;
            const auto numTileRows = (numOutputRows + tileSize - 1) / tileSize;
            const auto numTileColumns = (numOutputColumns + tileSize - 1) / tileSize;
            auto offset = (tileRow * numTileColumns * numChannels) + (tileColumn * numChannels) + channel;
            auto entryStride = numTileRows * numTileColumns * numChannels;

            for (int windowLoc = 0; windowLoc < windowSize * windowSize; ++windowLoc)
            {
                auto outputLoc = windowLoc * entryStride + offset;
                function.MemoryCopy<ValueType>(transformedInputBlock, function.Literal<int>(windowLoc * blockSize), transformedInput, outputLoc, function.Literal<int>(blockSize));
            }
        }

        template <typename ValueType>
        void ProcessInputBlock(emitters::IRFunctionEmitter& function,
                               llvm::Value* input,
                               emitters::IRLocalScalar tileRow,
                               emitters::IRLocalScalar tileColumn,
                               emitters::IRLocalScalar channelIndex,
                               int inputRowStride,
                               int numOutputRows,
                               int numOutputColumns,
                               int numChannels,
                               int tileSize,
                               int filterSize,
                               int blockSize,
                               llvm::Value* inputBlock,
                               llvm::Value* transformedInputBlock,
                               llvm::Value* transformedInput)
        {
            // input: inputImageRows x inputImageColumns x numChannels tensor
            // transformedInput is a windowSize x windowSize x tileRows x tileColumns x numChannels tensor containing the entire transformed input signal
            GetInputBlock<ValueType>(function, input, tileRow, tileColumn, channelIndex, inputRowStride, numChannels, numOutputRows, numOutputColumns, tileSize, filterSize, blockSize, inputBlock);

            TransformInputBlock<ValueType>(function, inputBlock, tileSize, filterSize, blockSize, transformedInputBlock);

            SplatTransformedInputBlock<ValueType>(function, transformedInputBlock, tileRow, tileColumn, channelIndex, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, blockSize, transformedInput);
        }

        template <typename ValueType>
        void GetTransformedOutputBlock(emitters::IRFunctionEmitter& function, llvm::Value* transformedOutput, emitters::IRLocalScalar tileRow, emitters::IRLocalScalar tileColumn, emitters::IRLocalScalar filterIndex, int numOutputRows, int numOutputColumns, int numFilters, int tileSize, int filterSize, int blockSize, llvm::Value* transformedOutputBlock)
        {
            const auto windowSize = tileSize + filterSize - 1;
            const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
            const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;
            const auto windowEntryStride = numFilters * numTileRows * numTileColumns;
            const auto tileIndex = tileRow * numTileColumns + tileColumn;
            const auto offset = (tileIndex * numFilters) + filterIndex;

            for (int windowLoc = 0; windowLoc < windowSize * windowSize; ++windowLoc)
            {
                auto inputLoc = windowLoc * windowEntryStride + offset;
                function.MemoryCopy<ValueType>(transformedOutput, inputLoc, transformedOutputBlock, function.Literal<int>(windowLoc * blockSize), function.Literal<int>(blockSize));
            }
        }

        template <typename ValueType>
        void SplatOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* outputTile, emitters::IRLocalScalar tileRow, emitters::IRLocalScalar tileColumn, emitters::IRLocalScalar filterIndex, int numOutputRows, int numOutputColumns, int numFilters, int tileSize, int blockSize, llvm::Value* output)
        {
            auto tileRowSize = tileSize;
            auto tileColumnSize = tileSize;

            // check for constant tile row on last row
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow)))
            {
                auto tileStart = GetConstantIntValue<int>(tileRow) * tileSize;
                tileRowSize = std::min(tileSize, numOutputRows - tileStart);
            }

            // check for constant tile column on last column
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn)))
            {
                auto tileStart = GetConstantIntValue<int>(tileColumn) * tileSize;
                tileColumnSize = std::min(tileSize, numOutputColumns - tileStart);
            }

            // rowIndex and columnIndex are the row and column indices within the tile
            const auto columnStride = numFilters;
            const auto rowStride = columnStride * numOutputColumns;
            auto outputStart = (tileSize * tileRow * rowStride) + (tileSize * tileColumn * columnStride) + filterIndex; // offset into the upper-left of the given tile
            for (int rowIndex = 0; rowIndex < tileRowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < tileColumnSize; ++columnIndex)
                {
                    auto inputLoc = ((rowIndex * tileSize) + columnIndex) * blockSize;
                    auto outputLoc = outputStart + (rowIndex * rowStride) + (columnIndex * columnStride);
                    function.MemoryCopy<ValueType>(outputTile, function.Literal<int>(inputLoc), output, outputLoc, function.Literal<int>(blockSize));
                }
            }
        }

        template <typename ValueType>
        void AccumulateOutputTile(emitters::IRFunctionEmitter& function, llvm::Value* outputTile, emitters::IRLocalScalar tileRow, emitters::IRLocalScalar tileColumn, emitters::IRLocalScalar filterIndex, int numOutputRows, int numOutputColumns, int numFilters, int tileSize, int blockSize, llvm::Value* output)
        {
            auto tileRowSize = tileSize;
            auto tileColumnSize = tileSize;

            // check for constant tile row on last row
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileRow)))
            {
                auto tileStart = GetConstantIntValue<int>(tileRow) * tileSize;
                tileRowSize = std::min(tileSize, numOutputRows - tileStart);
            }

            // check for constant tile column on last column
            if (llvm::isa<llvm::ConstantInt>(static_cast<llvm::Value*>(tileColumn)))
            {
                auto tileStart = GetConstantIntValue<int>(tileColumn) * tileSize;
                tileColumnSize = std::min(tileSize, numOutputColumns - tileStart);
            }

            // rowIndex and columnIndex are the row and column indices within the tile
            const auto columnStride = numFilters;
            const auto rowStride = columnStride * numOutputColumns;
            auto outputStart = (tileSize * tileRow * rowStride) + (tileSize * tileColumn * columnStride) + filterIndex; // offset into the upper-left of the given tile
            for (int rowIndex = 0; rowIndex < tileRowSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < tileColumnSize; ++columnIndex)
                {
                    auto inputLoc = ((rowIndex * tileSize) + columnIndex) * blockSize;
                    auto outputLoc = outputStart + (rowIndex * rowStride) + (columnIndex * columnStride);

                    auto outputTileArray = function.LocalArray(function.PointerOffset(outputTile, inputLoc));
                    auto outputArray = function.LocalArray(function.PointerOffset(output, outputLoc));
                    for (int blockEntryIndex = 0; blockEntryIndex < blockSize; ++blockEntryIndex)
                    {
                        outputArray[blockEntryIndex] = outputArray[blockEntryIndex] + outputTileArray[blockEntryIndex];
                    }
                }
            }
        }

        //
        // `ProcessOutputBlock()` copies data for a transformed output tile into the correct place in the output
        // If the tile row or column are passed in as compile-time constants, then partial tiles will be correctly
        // copied, otherwise this code assumes the tile is fully contained in the output.
        //
        template <typename ValueType>
        void ProcessOutputBlock(emitters::IRFunctionEmitter& function,
                                llvm::Value* transformedOutput,
                                emitters::IRLocalScalar tileRow,
                                emitters::IRLocalScalar tileColumn,
                                emitters::IRLocalScalar filterIndex,
                                int numOutputRows,
                                int numOutputColumns,
                                int numFilters,
                                int tileSize,
                                int filterSize,
                                int blockSize,
                                llvm::Value* transformedOutputBlock,
                                llvm::Value* outputTile,
                                llvm::Value* output)
        {
            // transformedOutput is a numTileRows, numTileColumns, numFilters image tensor containing the convolution result
            GetTransformedOutputBlock<ValueType>(function, transformedOutput, tileRow, tileColumn, filterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, filterSize, blockSize, transformedOutputBlock);

            TransformOutputBlock<ValueType>(function, transformedOutputBlock, tileSize, filterSize, blockSize, outputTile);

            // outputTile is the tile block at (tileRow, tileColumn, filterIndex) of the output
            SplatOutputTile<ValueType>(function, outputTile, tileRow, tileColumn, filterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, blockSize, output);
        }

        //
        // `ConvolveAccumulateBlock()` performs the Winograd convolution for a single block of the input data. This function is used
        // in 'filtersFirst' and separable convolutions.
        //
        template <typename ValueType>
        void ConvolveAccumulateBlock(emitters::IRFunctionEmitter& function,
                                     llvm::Value* input,
                                     llvm::Value* filterChannel,
                                     emitters::IRLocalScalar tileRow,
                                     emitters::IRLocalScalar tileColumn,
                                     emitters::IRLocalScalar channelIndex,
                                     emitters::IRLocalScalar filterIndex,
                                     int inputRowStride,
                                     int numOutputRows,
                                     int numOutputColumns,
                                     int numChannels,
                                     int numFilters,
                                     int tileSize,
                                     int filterSize,
                                     int blockSize,
                                     llvm::Value* inputBlock,
                                     llvm::Value* transformedInputBlock,
                                     llvm::Value* transformedOutputBlock,
                                     llvm::Value* outputTile,
                                     llvm::Value* output)
        {
            const auto windowSize = filterSize + tileSize - 1;

            // Compute X = B'dB
            GetInputBlock<ValueType>(function, input, tileRow, tileColumn, channelIndex, inputRowStride, numChannels, numOutputRows, numOutputColumns, tileSize, filterSize, blockSize, inputBlock);

            TransformInputBlock<ValueType>(function, inputBlock, tileSize, filterSize, blockSize, transformedInputBlock);

            ElementwiseMultiply(function, filterChannel, transformedInputBlock, windowSize * windowSize, transformedOutputBlock);

            // Now compute output tile Y = At * X * A
            TransformOutputBlock<ValueType>(function, transformedOutputBlock, tileSize, filterSize, blockSize, outputTile);

            // Accumulate the tile into the output
            AccumulateOutputTile<ValueType>(function, outputTile, tileRow, tileColumn, filterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, blockSize, output);
        }

        //
        // Core algorithm parts
        //
        template <typename ValueType>
        void TransformInput(emitters::IRFunctionEmitter& function,
                            llvm::Value* input,
                            int numOutputRows,
                            int numOutputColumns,
                            int numChannels,
                            int inputRowStride,
                            int tileSize,
                            int filterSize,
                            int blockSize,
                            llvm::Value* transformedInput)
        {
            const int windowSize = tileSize + filterSize - 1;
            const auto numFullTileRows = numOutputRows / tileSize;
            const auto numFullTileColumns = numOutputColumns / tileSize;
            const auto numTileRows = (numOutputRows + tileSize - 1) / tileSize;
            const auto numTileColumns = (numOutputColumns + tileSize - 1) / tileSize;

            // scratch space for conversion
            llvm::AllocaInst* inputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);
            llvm::AllocaInst* transformedInputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);

            // Get transformed input for all "full" windows (ones that don't fall off the edge or bottom of the input image)
            function.For(numFullTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index1) {
                auto rowTileIndex = function.LocalScalar(index1);
                function.For(numFullTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto columnTileIndex = function.LocalScalar(index2);
                    for (int channelIndex = 0; channelIndex < numChannels; channelIndex += blockSize)
                    {
                        int thisBlockSize = numChannels - channelIndex > blockSize ? blockSize : numChannels - channelIndex;
                        ProcessInputBlock<ValueType>(function, input, rowTileIndex, columnTileIndex, function.LocalScalar(channelIndex), inputRowStride, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, thisBlockSize, inputBlock, transformedInputBlock, transformedInput);
                    }
                });

                // Extra partial window at the right of the row
                if (numTileColumns > numFullTileColumns)
                {
                    auto columnTileIndex = function.LocalScalar(numFullTileColumns);
                    for (int channelIndex = 0; channelIndex < numChannels; channelIndex += blockSize)
                    {
                        int thisBlockSize = numChannels - channelIndex > blockSize ? blockSize : numChannels - channelIndex;
                        ProcessInputBlock<ValueType>(function, input, rowTileIndex, columnTileIndex, function.LocalScalar(channelIndex), inputRowStride, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, thisBlockSize, inputBlock, transformedInputBlock, transformedInput);
                    }
                }
            });

            // Extra row on the bottom
            if (numTileRows > numFullTileRows)
            {
                auto rowTileIndex = function.LocalScalar(numFullTileRows);
                function.For(numTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto columnTileIndex = function.LocalScalar(index2);
                    for (int channelIndex = 0; channelIndex < numChannels; channelIndex += blockSize)
                    {
                        int thisBlockSize = numChannels - channelIndex > blockSize ? blockSize : numChannels - channelIndex;
                        ProcessInputBlock<ValueType>(function, input, rowTileIndex, columnTileIndex, function.LocalScalar(channelIndex), inputRowStride, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, thisBlockSize, inputBlock, transformedInputBlock, transformedInput);
                    }
                });
            }

            // Extra entry in the bottom-right corner
            if ((numTileRows > numFullTileRows) && (numTileColumns > numFullTileColumns))
            {
                auto rowTileIndex = function.LocalScalar(numFullTileRows);
                auto columnTileIndex = function.LocalScalar(numFullTileColumns);
                for (int channelIndex = 0; channelIndex < numChannels; channelIndex += blockSize)
                {
                    int thisBlockSize = numChannels - channelIndex > blockSize ? blockSize : numChannels - channelIndex;
                    ProcessInputBlock<ValueType>(function, input, rowTileIndex, columnTileIndex, function.LocalScalar(channelIndex), inputRowStride, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, thisBlockSize, inputBlock, transformedInputBlock, transformedInput);
                }
            }
        }

        // Apply the (tranformed) filters to the transformed input to produce the transformed output
        template <typename ValueType>
        void ComputeTransformedOutput(emitters::IRFunctionEmitter& function,
                                      llvm::Value* transformedInput,
                                      llvm::Value* transformedFilters,
                                      int numOutputRows,
                                      int numOutputColumns,
                                      int numChannels,
                                      int numFilters,
                                      int tileSize,
                                      int filterSize,
                                      llvm::Value* transformedOutput)
        {
            // Do a matrix multiply to reduce many entries in parallel
            //
            // transformedSignal is a (windowRows*windowColumns) x (tr * tc) x (numChannels) tensor containing the entire transformed input signal
            // transformedFilters is a (windowRows*windowColumns) x (numFilters) x (numChannels) tensor
            // transformedOutput is a (windowRows*windowColumns) x (tr * tc) x (numFilters) tensor containing the entire transformed output signal

            const auto windowSize = filterSize + tileSize - 1;
            const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
            const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;

            // These strides are the distance between spatially-adjacent window entries in the various data structures
            int numOutputTiles = numTileRows * numTileColumns;
            int transformedInputStride = numOutputTiles * numChannels;
            int transformedFiltersStride = numFilters * numChannels;
            int transformedOutputStride = numOutputTiles * numFilters;

            // Each window pixel position has a separate matrix of values to transform via a matrix multiply
            for (int windowPosition = 0; windowPosition < windowSize * windowSize; ++windowPosition)
            {
                // Compute the offsets to the particular (wr, wc) matrix we want
                auto transformedInputMatrix = function.PointerOffset(transformedInput, windowPosition * transformedInputStride);
                auto transformedFiltersMatrix = function.PointerOffset(transformedFilters, windowPosition * transformedFiltersStride);
                auto transformedOutputMatrix = function.PointerOffset(transformedOutput, windowPosition * transformedOutputStride);

                // filter: m x k, input: k x n, output: m x n
                // transformedOutput = transformedFilter * transformedInput
                const int m = numOutputTiles;
                const int n = numFilters;
                const int k = numChannels;
                const int lda = numChannels;
                const int ldb = numChannels;
                const int ldc = numFilters;

                // Now do a matrix multiply to reduce many entries in parallel
                function.CallGEMM<ValueType>(false, true, m, n, k, transformedInputMatrix, lda, transformedFiltersMatrix, ldb, transformedOutputMatrix, ldc);
            }
        }

        template <typename ValueType>
        void TransformOutput(emitters::IRFunctionEmitter& function,
                             llvm::Value* transformedOutput,
                             int numOutputRows,
                             int numOutputColumns,
                             int numFilters,
                             int tileSize,
                             int filterSize,
                             int blockSize,
                             llvm::Value* output)
        {
            const int windowSize = tileSize + filterSize - 1;
            const auto numFullTileRows = numOutputRows / tileSize;
            const auto numFullTileColumns = numOutputColumns / tileSize;
            const auto numTileRows = (numOutputRows + tileSize - 1) / tileSize;
            const auto numTileColumns = (numOutputColumns + tileSize - 1) / tileSize;

            llvm::AllocaInst* transformedOutputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);
            llvm::AllocaInst* outputTile = function.Variable(emitters::GetVariableType<ValueType>(), tileSize * tileSize * blockSize);

            for (int filterIndex = 0; filterIndex < numFilters; filterIndex += blockSize)
            {
                auto irFilterIndex = function.LocalScalar<int>(filterIndex);

                int thisBlockSize = numFilters - filterIndex > blockSize ? blockSize : numFilters - filterIndex;
                function.For(numFullTileRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* index2) {
                    auto rowTileIndex = function.LocalScalar(index2);

                    // First output all of the complete tiles for this row
                    function.For(numFullTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto columnTileIndex = function.LocalScalar(index3);

                        ProcessOutputBlock<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, irFilterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, filterSize, thisBlockSize, transformedOutputBlock, outputTile, output);
                    });

                    // Extra partial tile on the right
                    if (numTileColumns > numFullTileColumns)
                    {
                        auto columnTileIndex = function.LocalScalar(numFullTileColumns);
                        ProcessOutputBlock<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, irFilterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, filterSize, thisBlockSize, transformedOutputBlock, outputTile, output);
                    }
                });

                // Extra row on the bottom
                if (numTileRows > numFullTileRows)
                {
                    auto rowTileIndex = function.LocalScalar(numFullTileRows);
                    function.For(numFullTileColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* index3) {
                        auto columnTileIndex = function.LocalScalar(index3);
                        ProcessOutputBlock<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, irFilterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, filterSize, thisBlockSize, transformedOutputBlock, outputTile, output);
                    });
                }

                // Extra entry in the lower-right partial tile
                if ((numTileRows > numFullTileRows) && (numTileColumns > numFullTileColumns))
                {
                    auto rowTileIndex = function.LocalScalar(numFullTileRows);
                    auto columnTileIndex = function.LocalScalar(numFullTileColumns);
                    ProcessOutputBlock<ValueType>(function, transformedOutput, rowTileIndex, columnTileIndex, irFilterIndex, numOutputRows, numOutputColumns, numFilters, tileSize, filterSize, thisBlockSize, transformedOutputBlock, outputTile, output);
                }
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
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(other._outputMemoryLayout)), _inputMemoryLayout(other._inputMemoryLayout), _outputMemoryLayout(other._outputMemoryLayout), _filterWeights(other._filterWeights), _stride(other._stride), _tileSize(other._tileSize), _filterSize(other._filterSize), _order(other._order)
    {
    }

    template <typename ValueType>
    WinogradConvolutionNode<ValueType>::WinogradConvolutionNode(const model::PortElements<ValueType>& input,
                                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                                const ConstTensorReferenceType& filterWeights,
                                                                int stride,
                                                                int tileSize,
                                                                FilterOrder order)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _stride(stride), _tileSize(tileSize), _order(order)
    {
        const int numFilters = outputMemoryLayout.GetActiveSize(2);
        _filterSize = filterWeights.NumColumns();
        if (filterWeights.NumRows() != static_cast<size_t>(_filterSize * numFilters))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "WinogradConvolutionComputeNode filterWeights.NumRows() != static_cast<size_t.(_filterSize * numFilters)");
        }
        _filterWeights = dsp::GetTransformedFilters(filterWeights, numFilters, _tileSize, order);
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
        const auto numFilterChannels = _order == FilterOrder::tilesFirst ? _filterWeights.NumChannels() : _filterWeights.NumColumns();
        auto convNode = transformer.AddNode<WinogradConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, _outputMemoryLayout, _stride, _tileSize, _filterSize, _order, static_cast<int>(numFilterChannels));
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
        archiver["order"] << to_string(_order);
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
        std::string orderName;
        archiver["order"] >> orderName;
        _order = filter_order_from_string(orderName);
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
                                                                              int filterSize,
                                                                              FilterOrder order,
                                                                              int numFilterChannels)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _stride(stride), _tileSize(tileSize), _filterSize(filterSize), _order(order), _numFilterChannels(numFilterChannels)
    {
        const auto numChannels = inputMemoryLayout.GetActiveSize(2);
        const int numFilters = outputMemoryLayout.GetActiveSize(2);
        _inputBlockSize = std::min(256, numChannels);
        _outputBlockSize = std::min(256, numFilters);
    }

    template <typename ValueType>
    WinogradConvolutionComputeNode<ValueType>::WinogradConvolutionComputeNode(const WinogradConvolutionComputeNode<ValueType>& other,
                                                                              const model::PortElements<ValueType>& input,
                                                                              const model::PortElements<ValueType>& filterWeights)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(other._outputMemoryLayout)), _inputMemoryLayout(other._inputMemoryLayout), _outputMemoryLayout(other._outputMemoryLayout), _stride(other._stride), _tileSize(other._tileSize), _filterSize(other._filterSize), _order(other._order), _numFilterChannels(other._numFilterChannels), _inputBlockSize(other._inputBlockSize), _outputBlockSize(other._outputBlockSize)
    {
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<WinogradConvolutionComputeNode<ValueType>>(*this, newInput, newFilterWeights);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* input = compiler.EnsurePortEmitted(this->input);
        llvm::Value* transformedFilters = compiler.EnsurePortEmitted(this->filterWeights);
        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

        switch (_order)
        {
        case FilterOrder::filtersFirst:
            CompileFiltersFirst(compiler, function, input, transformedFilters, output);
            break;
        case FilterOrder::tilesFirst:
            CompileTilesFirst(compiler, function, input, transformedFilters, output);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "WinogradConvolutionComputeNode has illegal order");
        }
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::CompileTilesFirst(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                      llvm::Value* input, llvm::Value* transformedFilters, llvm::Value* output)
    {
        auto& module = function.GetModule();

        // Input data parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto inputRowStride = inputLayout.GetCumulativeIncrement(0);
        const auto numChannels = inputLayout.GetActiveSize(2);
        assert((inputLayout.GetOffset(0) == _filterSize / 2) && "Padding must be filterSize/2");

        // Output data parameters
        const auto outputLayout = this->GetOutputMemoryLayout();
        const int numOutputRows = outputLayout.GetActiveSize(0);
        const int numOutputColumns = outputLayout.GetActiveSize(1);
        const int numFilters = outputLayout.GetActiveSize(2);

        // Winograd-specific stuff
        const auto windowSize = _filterSize + _tileSize - 1;
        const auto numTileRows = (numOutputRows + _tileSize - 1) / _tileSize;
        const auto numTileColumns = (numOutputColumns + _tileSize - 1) / _tileSize;

        // Allocate scratch space to hold transformed input and output
        int transformedInputSize = windowSize * windowSize * numTileRows * numTileColumns * numChannels;
        int transformedOutputSize = windowSize * windowSize * numTileRows * numTileColumns * numFilters;
        auto transformedInput = module.GlobalArray<ValueType>(GetInternalStateIdentifier() + "_transformedInput", transformedInputSize);
        auto transformedOutput = module.GlobalArray<ValueType>(GetInternalStateIdentifier() + "_transformedOutput", transformedOutputSize);

        // transformedInput is (windowSize*windowSize) x (tileRows * tileColumns) x numChannels
        // transformedFilters is (windowSize*windowSize) x numFilters x numChannels
        // transformedOutput is (windowSize*windowSize) x (tileRows * tileColumns) x numFilters

        // This is the core of the Winograd convolution algorithm: transform the input, perform an elementwise multiply between it an the transformed filter, and transform it back
        TransformInput<ValueType>(function, input, numOutputRows, numOutputColumns, numChannels, inputRowStride, _tileSize, _filterSize, _inputBlockSize, transformedInput);
        ComputeTransformedOutput<ValueType>(function, transformedInput, transformedFilters, numOutputRows, numOutputColumns, numChannels, numFilters, _tileSize, _filterSize, transformedOutput);
        TransformOutput<ValueType>(function, transformedOutput, numOutputRows, numOutputColumns, numFilters, _tileSize, _filterSize, _outputBlockSize, output);
    }

    template <typename ValueType>
    void WinogradConvolutionComputeNode<ValueType>::CompileFiltersFirst(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                        llvm::Value* input, llvm::Value* transformedFilters, llvm::Value* output)
    {
        const auto windowSize = _filterSize + _tileSize - 1;

        // Input data parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto inputRowStride = inputLayout.GetCumulativeIncrement(0);
        // const auto numInputRows = inputLayout.GetActiveSize(0);
        // const auto numInputColumns = inputLayout.GetActiveSize(1);
        const auto numChannels = inputLayout.GetActiveSize(2);
        assert((inputLayout.GetOffset(0) == _filterSize / 2) && "Padding must be filterSize/2");

        // Filter data parameters
        const int numFilterChannels = _numFilterChannels;
        const int filterStride = numFilterChannels * windowSize * windowSize;
        const int filterChannelStride = windowSize * windowSize;

        // Output data parameters
        const auto outputLayout = this->GetOutputMemoryLayout();
        const int numOutputRows = outputLayout.GetActiveSize(0);
        const int numOutputColumns = outputLayout.GetActiveSize(1);
        const int numFilters = outputLayout.GetActiveSize(2);

        // Winograd-specific parameters
        const auto numFullTileRows = numOutputRows / _tileSize;
        const auto numFullTileColumns = numOutputColumns / _tileSize;
        const auto numTileRows = (numOutputRows + _tileSize - 1) / _tileSize;
        const auto numTileColumns = (numOutputColumns + _tileSize - 1) / _tileSize;

        int blockSize = 1;

        // Temporaries
        llvm::AllocaInst* inputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);
        llvm::AllocaInst* transformedInputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);
        llvm::AllocaInst* transformedOutputBlock = function.Variable(emitters::GetVariableType<ValueType>(), windowSize * windowSize * blockSize);
        llvm::AllocaInst* outputTile = function.Variable(emitters::GetVariableType<ValueType>(), _tileSize * _tileSize * blockSize);

        function.For(numFilters, [transformedFilters, numFullTileRows, numFullTileColumns, numTileRows, numTileColumns, numChannels, numFilters, numFilterChannels, numOutputRows, numOutputColumns, inputRowStride, filterStride, filterChannelStride, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, input, output, this](emitters::IRFunctionEmitter& function, llvm::Value* i) {
            auto filterIndex = function.LocalScalar(i);
            auto channelStart = (filterIndex * numFilterChannels) % numChannels;

            function.For(numFilterChannels, [filterIndex, channelStart, transformedFilters, numFullTileRows, numFullTileColumns, numTileRows, numTileColumns, numChannels, numFilters, numOutputRows, numOutputColumns, inputRowStride, filterStride, filterChannelStride, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, input, output, this](emitters::IRFunctionEmitter& function, llvm::Value* j) {
                auto filterChannel = function.LocalScalar(j);
                auto channelIndex = channelStart + filterChannel;
                auto filterChannelPtr = function.PointerOffset(transformedFilters, filterIndex * filterStride + filterChannel * filterChannelStride);

                // Convolve all "full" windows (ones that don't fall off the edge or bottom of the input image)
                function.For(numFullTileRows, [channelIndex, filterIndex, numFullTileColumns, numTileColumns, filterChannelPtr, numChannels, numFilters, numOutputRows, numOutputColumns, inputRowStride, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, input, output, this](emitters::IRFunctionEmitter& function, llvm::Value* k) {
                    auto tileRowIndex = function.LocalScalar(k);
                    function.For(numFullTileColumns, [tileRowIndex, channelIndex, filterIndex, filterChannelPtr, numChannels, numFilters, numOutputRows, numOutputColumns, inputRowStride, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, input, output, this](emitters::IRFunctionEmitter& function, llvm::Value* l) {
                        auto tileColumnIndex = function.LocalScalar(l);
                        ConvolveAccumulateBlock<ValueType>(function, input, filterChannelPtr, tileRowIndex, tileColumnIndex, channelIndex, filterIndex, inputRowStride, numOutputRows, numOutputColumns, numChannels, numFilters, _tileSize, _filterSize, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, output);
                    });

                    // Get the extra partial window at the right of the row if present
                    if (numTileColumns > numFullTileColumns)
                    {
                        auto tileColumnIndex = function.LocalScalar(numFullTileColumns);
                        ConvolveAccumulateBlock<ValueType>(function, input, filterChannelPtr, tileRowIndex, tileColumnIndex, channelIndex, filterIndex, inputRowStride, numOutputRows, numOutputColumns, numChannels, numFilters, _tileSize, _filterSize, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, output);
                    }
                });

                // Extra row on the bottom
                if (numTileRows > numFullTileRows)
                {
                    auto tileRowIndex = function.LocalScalar(numFullTileRows);
                    function.For(numFullTileColumns, [tileRowIndex, channelIndex, filterIndex, filterChannelPtr, numChannels, numFilters, numOutputRows, numOutputColumns, inputRowStride, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, input, output, this](emitters::IRFunctionEmitter& function, llvm::Value* l) {
                        auto tileColumnIndex = function.LocalScalar(l);
                        ConvolveAccumulateBlock<ValueType>(function, input, filterChannelPtr, tileRowIndex, tileColumnIndex, channelIndex, filterIndex, inputRowStride, numOutputRows, numOutputColumns, numChannels, numFilters, _tileSize, _filterSize, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, output);
                    });
                }

                // Extra entry in the bottom-right corner
                if ((numTileRows > numFullTileRows) && (numTileColumns > numFullTileColumns))
                {
                    auto tileRowIndex = function.LocalScalar(numFullTileRows);
                    auto tileColumnIndex = function.LocalScalar(numFullTileColumns);
                    ConvolveAccumulateBlock<ValueType>(function, input, filterChannelPtr, tileRowIndex, tileColumnIndex, channelIndex, filterIndex, inputRowStride, numOutputRows, numOutputColumns, numChannels, numFilters, _tileSize, _filterSize, blockSize, inputBlock, transformedInputBlock, transformedOutputBlock, outputTile, output);
                }
            });
        });
    }

    // Explicit specializations
    template class WinogradConvolutionNode<float>;
    template class WinogradConvolutionNode<double>;
} // nodes
} // ell
