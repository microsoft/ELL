////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WinogradConvolution.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WinogradConvolution.h"
#include "SimpleConvolution.h"

// math
#include "MatrixOperations.h"

// utilities
#include "Debug.h"
#include "Exception.h"

// stl
#include <cassert>
#include <initializer_list>

namespace ell
{
namespace dsp
{
    namespace
    {
        // Helper function to avoid annoying double-to-float errors
        template <typename ValueType, typename ValueType2>
        math::RowMatrix<ValueType> MakeMatrix(std::initializer_list<std::initializer_list<ValueType2>> list)
        {
            auto numRows = list.size();
            auto numColumns = list.begin()->size();
            std::vector<ValueType> data;
            data.reserve(numRows * numColumns);
            for (const auto& row : list)
            {
                DEBUG_THROW(row.size() != numColumns, utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));
                std::transform(row.begin(), row.end(), std::back_inserter(data), [](ValueType2 x) { return static_cast<ValueType>(x); });
            }
            return math::RowMatrix<ValueType>(numRows, numColumns, data);
        }

        // Helper function for basic matrix multiplication
        template <typename ValueType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::MatrixLayout layout3>
        void Multiply(const math::ConstMatrixReference<ValueType, layout1>& A, const math::ConstMatrixReference<ValueType, layout2>& B, math::MatrixReference<ValueType, layout3>& C)
        {
            math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), A, B, static_cast<ValueType>(0.0), C);
        }
    }
    //
    // Explicit instantiations --- placed at the top of the file for readability
    //

    // Basic 1D entry points
    template math::RowVector<float> Convolve1DWinograd(const math::RowVector<float>& signal, const math::RowVector<float>& filter, WinogradAlgorithmVersion version);
    template math::RowVector<float> Convolve1DWinograd(const math::RowVector<float>& signal, const math::RowVector<float>& filter, int tileSize, WinogradAlgorithmVersion version);
    template math::RowVector<double> Convolve1DWinograd(const math::RowVector<double>& signal, const math::RowVector<double>& filter, WinogradAlgorithmVersion version);
    template math::RowVector<double> Convolve1DWinograd(const math::RowVector<double>& signal, const math::RowVector<double>& filter, int tileSize, WinogradAlgorithmVersion version);

    // Basic matrix-valued 2D entry points
    template math::RowMatrix<float> Convolve2DWinograd(const math::ConstRowMatrixReference<float>& signal, const math::ConstRowMatrixReference<float>& filter, WinogradAlgorithmVersion version);
    template math::RowMatrix<float> Convolve2DWinograd(const math::ConstRowMatrixReference<float>& signal, const math::ConstRowMatrixReference<float>& filter, int tileSize, WinogradAlgorithmVersion version);
    template math::RowMatrix<double> Convolve2DWinograd(const math::ConstRowMatrixReference<double>& signal, const math::ConstRowMatrixReference<double>& filter, WinogradAlgorithmVersion version);
    template math::RowMatrix<double> Convolve2DWinograd(const math::ConstRowMatrixReference<double>& signal, const math::ConstRowMatrixReference<double>& filter, int tileSize, WinogradAlgorithmVersion version);

    // Basic tensor-valued 2D entry points
    template math::ChannelColumnRowTensor<float> Convolve2DWinograd(const math::ChannelColumnRowTensor<float>& signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters, WinogradAlgorithmVersion version);
    template math::ChannelColumnRowTensor<float> Convolve2DWinograd(const math::ChannelColumnRowTensor<float>& signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters, int tileSize, WinogradAlgorithmVersion version);
    template math::ChannelColumnRowTensor<double> Convolve2DWinograd(const math::ChannelColumnRowTensor<double>& signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters, WinogradAlgorithmVersion version);
    template math::ChannelColumnRowTensor<double> Convolve2DWinograd(const math::ChannelColumnRowTensor<double>& signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters, int tileSize, WinogradAlgorithmVersion version);

    // Winograd implementation functions
    template math::RowMatrix<float> GetLeftDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetLeftFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetLeftResultTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightResultTransformMatrix(int tileSize, int filterSize);
    template math::ChannelColumnRowTensor<float> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<float> filters, int numFilters, int tileSize, WinogradAlgorithmVersion version);
    template void TransformFilters(math::ConstChannelColumnRowTensorReference<float> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<float> transformedFilters);
    template math::RowMatrix<double> GetLeftDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetLeftFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetLeftResultTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightResultTransformMatrix(int tileSize, int filterSize);
    template math::ChannelColumnRowTensor<double> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<double> filters, int numFilters, int tileSize, WinogradAlgorithmVersion version);
    template void TransformFilters(math::ConstChannelColumnRowTensorReference<double> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<double> transformedFilters);

    //
    // Declarations of implementation functions local to this file
    //

    // 1D
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinogradSlow(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize);

    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinogradFast(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize);

    // 2D
    template <typename ValueType>
    void Convolve2DWinogradV1(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& transformedFilter, int tileSize, math::RowMatrix<ValueType>& result);

    template <typename ValueType>
    void Convolve2DWinogradV1(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, math::ChannelColumnRowTensor<ValueType>& result);

    template <typename ValueType>
    void Convolve2DWinogradV2(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int windowSize, math::ChannelColumnRowTensor<ValueType>& result);

    template <typename ValueType>
    void Convolve2DWinogradFast(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& transformedFilter, int tileSize, math::RowMatrix<ValueType>& result);

    //
    // Utility functions
    //

    // Winograd convolution works by tiling the input into a set of 'input tiles' or 'windows' and
    // performing a low-operation-count convolution of that window with the filter, producing an output
    // tile. The input tiles must be larger than the output tiles, and overlap. The input tile size is
    // determined by the output tile size and the filter size, and Winograd kernels are referred to using
    // those parameters. For instance, in 1D, F(2,3) is the filtering algorithm for a size 2 output tile and a
    // size 3 filter. The 2D equivalent is F(2x2, 3x3), which produces a 2x2 output tile using a 3x3 filter.
    // The relationship between output tile size, filter size, and input tile (window) size is:
    //
    //     window_size = tile_size + filter_size - 1
    //
    // Therefore, the F(2,3) algorithm requires (2+3-1), or 4, inputs to produce its 2-output tile.
    //
    // The algorithm uses three matrices to perform an algebraic transformation from the input into
    // a representation that enables performing the convolution with fewer operations. These matrices
    // are:
    //
    // B': the input transformation matrix. A square matrix of dimension window_size x window_size that
    //     is multiplied with the input tile.
    //
    // G:  the filter transformation matrix. A rectangular matrix of dimension window_size x filter_size
    //     that is multiplied with the filter, transforming it into something of dimension window_size
    //
    // A': the output transformation matrix. A rectangular matrix of dimension tile_size x window_size that
    //     transforms intermediate results into output tiles
    //
    // Usage:
    //
    // To convolve an input tile with the filter, we first use the B' and G matrices to transform the filter
    // and the input into window_size pieces. Then perform elementwise multiplication between the two, and
    // finally transform that result into an output tile with the A' matrix:
    //
    //     Y = d (*) g             (the input signal 'd' convolved with the filter 'g')
    //       = A' * (Gg .* B'd)    (where '.*' denotes elementwise multiplication)
    //
    //     d = input signal (e.g., [d0 d1 d2 d3]')
    //     g = filter (e.g., [g0 g1 g2]')
    //
    // To perform 2D convolution, we start with 2D input tile d and 2D input filter g,
    // and transform the data by multiplying on the left by the transform matrix and on
    // the right by its transpose:
    //
    //     Y = d (*) g
    //       = A' * (GgG' .* B'dB) * A
    //
    //
    // I don't know why the input and output transformation matrices are called B' and A' instead
    // of just B and A.
    //
    //
    // The matrices:
    //
    // For F(2,3)
    //
    //       1   0  -1   0
    // B' =  0   1   1   0
    //       0  -1   1   0
    //       0   1   0  -1
    //
    //        1     0     0
    // G =  1/2   1/2   1/2
    //      1/2  -1/2   1/2
    //        0     0     1
    //
    // A' =  1   1   1   0
    //       0   1  -1  -1
    //
    //
    // For F(4,3)
    //
    //      4   0  -5   0   1   0
    //      0  -4  -4   1   1   0
    // B' = 0   4  -4  -1   1   0
    //      0  -2  -1   2   1   0
    //      0   2  -1  -2   1   0
    //      0   4   0  -5   0   1
    //
    //
    //       1/4      0      0
    //      -1/6   -1/6   -1/6
    // G =  -1/6    1/6   -1/6
    //      1/24   1/12    1/6
    //      1/24  -1/12    1/6
    //         0      0      1
    //
    //
    //       1   1   1   1   1   0
    // A' =  0   1  -1   2  -2   0
    //       0   1   1   4   4   0
    //       0   1  -1   8  -8   1
    //

    /// <summary> Gets the data-transforming matrix for Winograd convolution (commonly notated as B') </summary>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftDataTransformMatrix(int tileSize, int filterSize)
    {
        if (tileSize == 2 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ { 1,  0, -1,  0 },
                                           { 0,  1,  1,  0 },
                                           { 0, -1,  1,  0 },
                                           { 0,  1,  0, -1 } });
            // clang-format on
        }
        if (tileSize == 4 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ { 4,  0, -5,  0,  1,  0 },
                                           { 0, -4, -4,  1,  1,  0 },
                                           { 0,  4, -4, -1,  1,  0 },
                                           { 0, -2, -1,  2,  1,  0 },
                                           { 0,  2, -1, -2,  1,  0 },
                                           { 0,  4,  0, -5,  0,  1 } });
            // clang-format on
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightDataTransformMatrix(int tileSize, int filterSize)
    {
        return { GetLeftDataTransformMatrix<ValueType>(tileSize, filterSize).Transpose() };
    }

    /// <summary> Gets the filter-transforming matrix for Winograd convolution (commonly notated as G) </summary>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftFilterTransformMatrix(int tileSize, int filterSize)
    {
        if (tileSize == 2 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ {     1.0,      0.0,     0.0 }, 
                                           { 1.0 / 2,  1.0 / 2, 1.0 / 2 }, 
                                           { 1.0 / 2, -1.0 / 2, 1.0 / 2 }, 
                                           {     0.0,      0.0,     1.0 } });
            // clang-format on
        }
        if (tileSize == 4 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ {  1.0 / 4,       0.0,       0.0 },
                                           { -1.0 / 6,  -1.0 / 6,  -1.0 / 6 },
                                           { -1.0 / 6,   1.0 / 6,  -1.0 / 6 },
                                           {  1.0 / 24,  1.0 / 12,  1.0 / 6 },
                                           {  1.0 / 24, -1.0 / 12,  1.0 / 6 },
                                           {       0.0,       0.0,      0.0 } });
            // clang-format on
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightFilterTransformMatrix(int tileSize, int filterSize)
    {
        return { GetLeftFilterTransformMatrix<ValueType>(tileSize, filterSize).Transpose() };
    }

    /// <summary> Gets the result-transforming matrix for Winograd convolution (commonly notated as A') </summary>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftResultTransformMatrix(int tileSize, int filterSize)
    {
        if (tileSize == 2 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ { 1,  1,  1,  0 },
                                           { 0,  1, -1, -1 } });
            // clang-format on
        }
        if (tileSize == 4 && filterSize == 3)
        {
            // clang-format off
            return MakeMatrix<ValueType>({ { 1,  1,  1,  1,  1,  0 },
                                           { 0,  1, -1,  2, -2,  0 },
                                           { 0,  1,  1,  4,  4,  0 },
                                           { 0,  1, -1,  8, -8,  1 } });
            // clang-format on
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightResultTransformMatrix(int tileSize, int filterSize)
    {
        return { GetLeftResultTransformMatrix<ValueType>(tileSize, filterSize).Transpose() };
    }

    //
    // Utility functions for extracting data from or inserting data into tensors
    //

    // Extract a non-contiguous slice from a tensor by copying
    template <typename ValueType>
    void GetChannelSlice(math::ConstChannelColumnRowTensorReference<ValueType> tensor, int channelIndex, math::RowMatrix<ValueType>& slice)
    {
        const auto numRows = static_cast<int>(tensor.NumRows());
        const auto numColumns = static_cast<int>(tensor.NumColumns());
        assert(numRows == static_cast<int>(slice.NumRows()));
        assert(numColumns == static_cast<int>(slice.NumColumns()));
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < numColumns; ++columnIndex)
            {
                slice(rowIndex, columnIndex) = tensor(rowIndex, columnIndex, channelIndex);
            }
        }
    }

    // Gather wr x wc slice from transformed input tensor
    // transformedInputTensor is (wr*wc) x nf x (tr * tc), tr and rc being the output size, in tiles
    // output is (wr*wc) matrix
    template <typename ValueType>
    void GetWindowSlice(math::ConstChannelColumnRowTensorReference<ValueType> transformedInputTensor, int tileRowIndex, int tileColumnIndex, int filterIndex, int tileRows, int tileColumns, math::RowMatrix<ValueType>& slice)
    {
        UNUSED(tileRows);
        auto numWindowRows = static_cast<int>(slice.NumRows());
        auto numWindowColumns = static_cast<int>(slice.NumRows());
        assert(numWindowRows * numWindowColumns == static_cast<int>(transformedInputTensor.NumRows()));
        assert(filterIndex < static_cast<int>(transformedInputTensor.NumColumns()));
        for (int windowRowIndex = 0; windowRowIndex < numWindowRows; ++windowRowIndex)
        {
            for (int windowColumnIndex = 0; windowColumnIndex < numWindowColumns; ++windowColumnIndex)
            {
                slice(windowRowIndex, windowColumnIndex) = transformedInputTensor((windowRowIndex * numWindowColumns) + windowColumnIndex, filterIndex, tileRowIndex * tileColumns + tileColumnIndex);
            }
        }
    }

    template <typename ValueType>
    void SplatFilterTile(const math::ConstRowMatrixReference<ValueType>& transformedFilterTile, int filterIndex, int channelIndex, math::ChannelColumnRowTensorReference<ValueType>& transformedFilters)
    {
        // transformedtransformedFilterTile is a wr x wc slice: channel `channelIndex` of the filter at `filterIndex`
        // transformedFilters is a (wr*wc) x nf x d tensor containing the entire set of filters
        const auto numWindowRows = static_cast<int>(transformedFilterTile.NumRows());
        const auto numWindowColumns = static_cast<int>(transformedFilterTile.NumColumns());
        assert(static_cast<int>(transformedFilters.NumRows()) == numWindowRows * numWindowColumns);
        for (int windowRowIndex = 0; windowRowIndex < numWindowRows; ++windowRowIndex)
        {
            for (int windowColumnIndex = 0; windowColumnIndex < numWindowColumns; ++windowColumnIndex)
            {
                transformedFilters((windowRowIndex * numWindowColumns) + windowColumnIndex, filterIndex, channelIndex) = transformedFilterTile(windowRowIndex, windowColumnIndex);
            }
        }
    }

    template <typename ValueType>
    void SplatDataTile(const math::RowMatrix<ValueType>& dataTile, int tileRowIndex, int tileColumnIndex, int channelIndex, int numTileRows, int numTileColumns, math::ChannelColumnRowTensorReference<ValueType>& transformedSignal)
    {
        UNUSED(numTileRows);

        // tr, tc are tile indices: r/tileSize and c/tileSize
        // dataTile is a wr x wc matrix
        // transformedSignal is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
        const auto numWindowRows = static_cast<int>(dataTile.NumRows());
        const auto numWindowColumns = static_cast<int>(dataTile.NumColumns());
        assert(static_cast<int>(transformedSignal.NumRows()) == numWindowRows * numWindowColumns);

        for (int windowRowIndex = 0; windowRowIndex < numWindowRows; ++windowRowIndex)
        {
            for (int windowColumnIndex = 0; windowColumnIndex < numWindowColumns; ++windowColumnIndex)
            {
                transformedSignal((windowRowIndex * numWindowColumns) + windowColumnIndex, channelIndex, (tileRowIndex * numTileColumns) + tileColumnIndex) = dataTile(windowRowIndex, windowColumnIndex);
            }
        }
    }

    // outputTile is a tr x tc matrix
    // result is a r x c x nf tensor
    template <typename ValueType>
    void SplatOutputTile(const math::RowMatrix<ValueType>& outputTile, int tileRowIndex, int tileColumnIndex, int filterIndex, int tileSize, math::ChannelColumnRowTensorReference<ValueType>& result)
    {
        assert(static_cast<int>(outputTile.NumRows()) == tileSize && static_cast<int>(outputTile.NumColumns()) == tileSize);
        // iterate over entries in the tile
        for (int rowIndex = 0; rowIndex < tileSize; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < tileSize; ++columnIndex)
            {
                result((tileRowIndex * tileSize) + rowIndex, (tileColumnIndex * tileSize) + columnIndex, filterIndex) = outputTile(rowIndex, columnIndex);
            }
        }
    }

    // tile is a r x c slice
    // result is a r x c x d tensor
    template <typename ValueType>
    void CopyTile(const math::ConstRowMatrixReference<ValueType>& tile, int channelIndex, math::ChannelColumnRowTensorReference<ValueType>& result)
    {
        const auto numRows = static_cast<int>(tile.NumRows());
        const auto numColumns = static_cast<int>(tile.NumColumns());
        assert(numRows == static_cast<int>(result.NumRows()));
        assert(numColumns == static_cast<int>(result.NumColumns()));
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < numColumns; ++columnIndex)
            {
                result(rowIndex, columnIndex, channelIndex) = tile(rowIndex, columnIndex);
            }
        }
    }

    // tile is a tr x tc slice
    // result is a r x c x d tensor
    template <typename ValueType>
    void AccumulateTile(const math::ConstRowMatrixReference<ValueType>& tile, int rowOffset, int columnOffset, int channelOffset, math::ChannelColumnRowTensor<ValueType>& result)
    {
        const auto numRows = static_cast<int>(tile.NumRows());
        const auto numColumns = static_cast<int>(tile.NumColumns());
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < numColumns; ++columnIndex)
            {
                result(rowOffset + rowIndex, columnOffset + columnIndex, channelOffset) += tile(rowIndex, columnIndex);
            }
        }
    }

    // Notation:
    //
    // Input image: r x c x d
    // filters: nf x fr x fc x d
    // windows: wr x wc
    // output tiles: tr x tc
    //
    // r: input rows
    // c: input columns
    // d: input channels
    // fr: filter rows
    // fc: filter columns
    // nf: num filters

    template <typename ValueType>
    void TransformFilters(math::ConstChannelColumnRowTensorReference<ValueType> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<ValueType> transformedFilters)
    {
        using Matrix = math::RowMatrix<ValueType>;

        // filters is a (nf*fr) x fc x d tensor
        // transformedFilters is a (wr*wc) x nf x d tensor
        const auto filterSize = static_cast<int>(filters.NumRows()) / numFilters;
        const auto windowSize = tileSize + filterSize - 1;
        const auto numChannels = static_cast<int>(filters.NumChannels());
        assert(static_cast<int>(filters.NumColumns()) == filterSize);
        assert(static_cast<int>(transformedFilters.NumRows()) == windowSize * windowSize);
        assert(static_cast<int>(transformedFilters.NumColumns()) == numFilters);
        assert(static_cast<int>(transformedFilters.NumChannels()) == numChannels);

        // Precompute GgG', the transformed filter:
        Matrix G = GetLeftFilterTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix Gt = GetRightFilterTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix Gg(windowSize, filterSize);

        // for each "pixel" in a tile, we want to generate a nf x d matrix
        // we gather these matrices into a (windowSize*windowSize) x nf x d tensor

        // Temporaries to get slices
        Matrix filterSlice(filterSize, filterSize);
        Matrix transformedFilterSlice(windowSize, windowSize);

        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            auto filter = filters.GetSubTensor(filterIndex * filterSize, 0, 0, filterSize, filterSize, numChannels);
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                // u = Gg_(k,c)Gt   (a windowSize x windowSize matrix)
                GetChannelSlice(filter, channelIndex, filterSlice);

                Multiply(G, filterSlice, Gg);
                Multiply(Gg, Gt, transformedFilterSlice);
                SplatFilterTile(transformedFilterSlice, filterIndex, channelIndex, transformedFilters);
            }
        }
    }

    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<ValueType> filters, int numFilters, int tileSize, WinogradAlgorithmVersion version)
    {
        using Matrix = math::RowMatrix<ValueType>;
        using Tensor = math::ChannelColumnRowTensor<ValueType>;
        const auto filterSize = static_cast<int>(filters.NumRows()) / numFilters;
        const auto windowSize = tileSize + filterSize - 1;
        const auto numChannels = static_cast<int>(filters.NumChannels());

        // The two algorithm versions use different orderings of the transformed filter tensor
        if (version == WinogradAlgorithmVersion::v1)
        {
            // Temporaries to get slices
            Matrix filterSlice(filterSize, filterSize);
            Matrix transformedFilterSlice(windowSize, windowSize);

            // Precompute GgG', the transformed filter:
            Matrix G = GetLeftFilterTransformMatrix<ValueType>(tileSize, filterSize);
            Matrix Gt = GetRightFilterTransformMatrix<ValueType>(tileSize, filterSize);
            Matrix Gg(windowSize, filterSize);

            // transformedFilters is a (nf * wr) x wc x d, where wr == wc == windowSize
            auto transformedFilters = Tensor(numFilters * windowSize, windowSize, numChannels);
            // Transform the filters
            for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
            {
                // since the filters are stacked row-wise, we just skip down rows to get to the filter we want
                auto filter = filters.GetSubTensor(filterIndex * filterSize, 0, 0, filterSize, filterSize, numChannels);
                // transformedFilter is a windowSize x windowSize x numChannels tensor
                auto transformedFilter = transformedFilters.GetSubTensor(filterIndex * windowSize, 0, 0, windowSize, windowSize, numChannels);
                for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                {
                    GetChannelSlice(filter, channelIndex, filterSlice);
                    Multiply(G, filterSlice, Gg);
                    Multiply(Gg, Gt, transformedFilterSlice);
                    CopyTile(transformedFilterSlice, channelIndex, transformedFilter);
                }
            }

            return transformedFilters;
        }
        else if (version == WinogradAlgorithmVersion::v2)
        {
            // input filters tensor is (nf*fr) x fc x d
            // transformed filters is a (nf * wr) x wc x d tensor, where wr == wc == windowSize
            math::ChannelColumnRowTensor<ValueType> transformedFilters(windowSize * windowSize, numFilters, numChannels);
            TransformFilters(filters, numFilters, tileSize, transformedFilters);
            return transformedFilters;
        }
        else
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
    }

    //
    // Winograd convolution
    //
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, WinogradAlgorithmVersion version)
    {
        const int tileSize = 2;
        return Convolve1DWinograd(signal, filter, tileSize, version);
    }

    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize, WinogradAlgorithmVersion version)
    {
        math::RowVector<ValueType> result;
        switch (version)
        {
        case WinogradAlgorithmVersion::v1:
            result = Convolve1DWinogradSlow(signal, filter, tileSize);
            break;
        case WinogradAlgorithmVersion::v2:
            result = Convolve1DWinogradFast(signal, filter, tileSize);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        // Finish last bit not covered by a tile
        const int outputSize = static_cast<int>(result.Size());
        const auto numTiles = outputSize / tileSize;
        const int filterSize = static_cast<int>(filter.Size());
        if (numTiles * tileSize != outputSize)
        {
            auto startOutputIndex = numTiles * tileSize;
            auto extraEntries = outputSize - startOutputIndex;

            // Get relevant subvectors of input and output
            auto signalSubvector = signal.GetSubVector(startOutputIndex, filterSize + extraEntries - 1);
            auto resultSubvector = result.GetSubVector(startOutputIndex, extraEntries);
            Convolve1DSimple(signalSubvector, filter, resultSubvector);
        }

        return result;
    }

    //
    // Classes used for implementing Winograd convolution for sizes known at compile time
    //

    template <typename ValueType, int tileSize, int filterSize>
    struct FixedWinograd1D;

    template <typename ValueType>
    struct FixedWinograd1D<ValueType, 2, 3>
    {
        static math::RowVector<ValueType> ConvolveSlow(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter)
        {
            constexpr int tileSize = 2;
            constexpr int filterSize = 3;
            const auto outputSize = static_cast<int>(signal.Size()) - filterSize + 1;
            math::RowVector<ValueType> result(outputSize);

            // Simple version that uses F(2,3) and just computes blocks of size 2 sequentially
            math::RowVector<ValueType> outputTile(tileSize);
            int numTiles = outputSize / tileSize;
            for (int tileIndex = 0; tileIndex < numTiles; ++tileIndex)
            {
                int index = tileIndex * tileSize;
                auto d = &signal[index];
                auto m1 = (d[0] - d[2]) * filter[0];
                auto m2 = (d[1] + d[2]) * (filter[0] + filter[1] + filter[2]) / static_cast<ValueType>(2.0);
                auto m3 = (d[2] - d[1]) * (filter[0] - filter[1] + filter[2]) / static_cast<ValueType>(2.0);
                auto m4 = (d[1] - d[3]) * filter[2];
                result[index] = m1 + m2 + m3;
                result[index + 1] = m2 - m3 - m4;
            }
            return result;
        }

        static math::RowVector<ValueType> ConvolveFast(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter)
        {
            constexpr int tileSize = 2;
            constexpr int filterSize = 3;
            const auto outputSize = static_cast<int>(signal.Size()) - filterSize + 1;
            math::RowVector<ValueType> result(outputSize);

            // Y = A' * (Gg .* B'd)
            //
            // g = filter ([g0 g1 g2]')
            // d = signal ([d0 d1 d2 d3]')
            //
            //        1     0     0
            // G =  1/2   1/2   1/2
            //      1/2  -1/2   1/2
            //        0     0     1
            //
            //
            //       1   0  -1   0
            // B' =  0   1   1   0
            //       0  -1   1   0
            //       0   1   0  -1
            //
            //
            // A' =  1   1   1   0
            //       0   1  -1  -1
            //
            //
            // Precompute Gg:
            //
            //            g0
            // Gg = (g0 + g1 + g2) / 2
            //      (g0 - g1 + g2) / 2
            //            g2

            auto Gg0 = filter[0];
            auto Gg1 = (filter[0] + filter[1] + filter[2]) / static_cast<ValueType>(2);
            auto Gg2 = (filter[0] - filter[1] + filter[2]) / static_cast<ValueType>(2);
            auto Gg3 = filter[2];

            // prefetch 2 values so we don't have to get them again (check to see if this really matters)
            auto d = signal.GetConstDataPointer();
            ValueType d0 = d[0];
            ValueType d1 = d[1];
            int numTiles = outputSize / tileSize;
            for (int tileIndex = 0; tileIndex < numTiles; ++tileIndex)
            {
                int index = tileIndex * tileSize;
                ValueType d2 = d[2];
                ValueType d3 = d[3];

                // elementwise vector multiply
                auto m1 = (d0 - d2) * Gg0;
                auto m2 = (d1 + d2) * Gg1;
                auto m3 = (d2 - d1) * Gg2;
                auto m4 = (d1 - d3) * Gg3;
                result[index] = m1 + m2 + m3;
                result[index + 1] = m2 - m3 - m4;
                d += tileSize;

                // shift d0, d1
                d0 = d2;
                d1 = d3;
            }

            return result;
        }
    };

    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinogradSlow(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize)
    {
        using namespace std::string_literals;
        const int filterSize = static_cast<int>(filter.Size());
        if (tileSize == 2 && filterSize == 3)
        {
            return FixedWinograd1D<ValueType, 2, 3>::ConvolveSlow(signal, filter);
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "1D Winograd convolution not implemented for tile size "s + std::to_string(tileSize) + " and filter size " + std::to_string(filterSize));
    }

    // Matrix-based Winograd convolution
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinogradFast(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize)
    {
        using namespace std::string_literals;
        const int filterSize = static_cast<int>(filter.Size());
        if (tileSize == 2 && filterSize == 3)
        {
            return FixedWinograd1D<ValueType, 2, 3>::ConvolveFast(signal, filter);
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "1D Winograd convolution not implemented for tile size "s + std::to_string(tileSize) + " and filter size " + std::to_string(filterSize));
    }

    //
    // 2D convolution
    //

    // Y = A' * (GgG' .* B'dB) * A
    //
    // g = filter ([g0 g1 g2]')
    // d = signal ([d0 d1 d2 d3]')
    //
    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DWinograd(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& filter, WinogradAlgorithmVersion version)
    {
        // TODO: have an oracle or something to get tile size from: GetTileSize(signal, filter, version)
        const int tileSize = 2;
        return Convolve2DWinograd(signal, filter, tileSize, version);
    }

    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, WinogradAlgorithmVersion version)
    {
        const int tileSize = 2;
        return Convolve2DWinograd(signal, filters, numFilters, tileSize, version);
    }

    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DWinograd(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& filter, int tileSize, WinogradAlgorithmVersion version)
    {
        const auto filterSize = static_cast<int>(filter.NumRows());
        assert(filterSize == static_cast<int>(filter.NumColumns()) && "Filters must be square");
        const auto windowSize = tileSize + filterSize - 1;
        const auto outputRows = signal.NumRows() - filterSize + 1;
        const auto outputColumns = signal.NumColumns() - filterSize + 1;
        math::RowMatrix<ValueType> result(outputRows, outputColumns);

        // Precompute GgG', the transformed filter:
        math::RowMatrix<ValueType> G = GetLeftFilterTransformMatrix<ValueType>(tileSize, filterSize);
        math::RowMatrix<ValueType> Gt = GetRightFilterTransformMatrix<ValueType>(tileSize, filterSize);
        math::RowMatrix<ValueType> Gg(windowSize, filterSize);
        math::RowMatrix<ValueType> transformedFilter(windowSize, windowSize);
        Multiply(G, filter, Gg);
        Multiply(Gg, Gt, transformedFilter);

        switch (version)
        {
        case WinogradAlgorithmVersion::v1:
            result.Fill(0);
            Convolve2DWinogradV1(signal, transformedFilter, tileSize, result);
            break;
        case WinogradAlgorithmVersion::v2:
            Convolve2DWinogradFast(signal, transformedFilter, tileSize, result);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        // Compute any edge values not taking up a full tile increment, using "simple" algorithm
        const auto numTileRows = outputRows / tileSize;
        const auto numTileColumns = outputColumns / tileSize;
        if (numTileRows * tileSize != outputRows)
        {
            auto startOutputRow = numTileRows * tileSize;
            auto extraRows = outputRows - startOutputRow;

            // Get relevant subvectors of input and output
            auto signalSubmatrix = signal.GetSubMatrix(startOutputRow, 0, extraRows + filterSize - 1, signal.NumColumns());
            auto resultSubmatrix = result.GetSubMatrix(startOutputRow, 0, extraRows, outputColumns);
            Convolve2DSimple(signalSubmatrix, filter, resultSubmatrix);
        }

        if (numTileColumns * tileSize != outputColumns)
        {
            // The previous bit of cleanup on extra rows will have taken care of the bottom-right corner, where
            // there are both extra rows and extra columns. So, we only need to take care of the extra columns to the
            // right of the tiled area.
            auto outputRowsUsed = numTileRows * tileSize;
            auto inputRowsUsed = outputRowsUsed + filterSize - 1;

            auto startOutputColumn = numTileColumns * tileSize;
            auto extraColumns = outputColumns - startOutputColumn;

            // Get relevant subvectors of input and output
            auto signalSubmatrix = signal.GetSubMatrix(0, startOutputColumn, inputRowsUsed, extraColumns + filterSize - 1);
            auto resultSubmatrix = result.GetSubMatrix(0, startOutputColumn, outputRowsUsed, extraColumns);
            Convolve2DSimple(signalSubmatrix, filter, resultSubmatrix);
        }
        return result;
    }

    // filters is a nf x fr x fc x d tensor (represented in 3D as (nf*fr) x fc x d )
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, int tileSize, WinogradAlgorithmVersion version)
    {
        using Tensor = math::ChannelColumnRowTensor<ValueType>;

        const auto filterSize = static_cast<int>(filters.NumRows()) / numFilters;
        assert(filterSize == static_cast<int>(filters.NumColumns()) && "Filters must be square");
        const auto windowSize = tileSize + filterSize - 1;
        const auto outputRows = static_cast<int>(signal.NumRows()) - filterSize + 1;
        const auto outputColumns = static_cast<int>(signal.NumColumns()) - filterSize + 1;
        Tensor result(outputRows, outputColumns, numFilters);

        // The dimensions of transformedFilters depends on which version of the algorithm we're using
        Tensor transformedFilters(0, 0, 0);

        // In version 1, transformedFilters is a (nf * wr) x wc x d, where wr == wc == windowSize
        // In version 2, transformedFilters is a (wr*wc) x nf x d tensor
        transformedFilters = GetTransformedFilters(filters, numFilters, tileSize, version);

        switch (version)
        {
        case WinogradAlgorithmVersion::v1:
            // in version 1, the window size == # columns in the transformed filter tensor, so
            // we don't need to pass it in explicitly
            Convolve2DWinogradV1(signal, transformedFilters, numFilters, tileSize, result);
            break;
        case WinogradAlgorithmVersion::v2:
            Convolve2DWinogradV2(signal, transformedFilters, numFilters, tileSize, windowSize, result);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        // Compute any edge values not taking up a full tile increment, using "simple" algorithm
        const auto numTileRows = outputRows / tileSize;
        const auto numTileColumns = outputColumns / tileSize;
        if (numTileRows * tileSize != outputRows)
        {
            auto startOutputRow = numTileRows * tileSize;
            auto extraRows = outputRows - startOutputRow;

            // Get relevant subvectors of input and output
            auto signalSubtensor = signal.GetSubTensor(startOutputRow, 0, 0, extraRows + filterSize - 1, signal.NumColumns(), signal.NumChannels());
            auto resultSubtensor = result.GetSubTensor(startOutputRow, 0, 0, extraRows, outputColumns, numFilters);
            Convolve2DSimple(signalSubtensor, filters, numFilters, resultSubtensor);
        }

        if (numTileColumns * tileSize != outputColumns)
        {
            // The previous bit of cleanup on extra rows will have taken care of the bottom-right corner, where
            // there are both extra rows and extra columns. So, we only need to take care of the extra columns to the
            // right of the tiled area.
            auto outputRowsUsed = numTileRows * tileSize;
            auto inputRowsUsed = outputRowsUsed + filterSize - 1;

            auto startOutputColumn = numTileColumns * tileSize;
            auto extraColumns = outputColumns - startOutputColumn;

            // Get relevant subvectors of input and output
            auto signalSubtensor = signal.GetSubTensor(0, startOutputColumn, 0, inputRowsUsed, extraColumns + filterSize - 1, signal.NumChannels());
            auto resultSubtensor = result.GetSubTensor(0, startOutputColumn, 0, outputRowsUsed, extraColumns, numFilters);
            Convolve2DSimple(signalSubtensor, filters, numFilters, resultSubtensor);
        }

        return result;
    }

    //
    // TODO: generalize functions to operate on a subset of the spatial dimensions. Then the cleanup along the bottom and right edges will be easy.
    //

    //
    // Straightforward implementation of Winograd algorithm, using matrix multiplies for the data- and tile- transform steps
    //
    template <typename ValueType>
    void Convolve2DWinogradV1(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& transformedFilter, int tileSize, math::RowMatrix<ValueType>& result)
    {
        using Matrix = math::RowMatrix<ValueType>;

        const auto windowSize = static_cast<int>(transformedFilter.NumRows());
        const auto filterSize = windowSize - tileSize + 1;
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());

        // Get transform matrices
        Matrix Bt = GetLeftDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix B = GetRightDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix At = GetLeftResultTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix A = GetRightResultTransformMatrix<ValueType>(tileSize, filterSize);

        // Temporary values
        Matrix Btd(windowSize, windowSize); // Bt: 4x4, d: 4x4
        Matrix AtX(tileSize, windowSize); // At: 2x4, X: 4x4
        Matrix outputValues(tileSize, tileSize); // At: 2x4, X: 4x4
        Matrix BtdB(windowSize, windowSize);

        int rowTiles = outputRows / tileSize;
        int columnTiles = outputColumns / tileSize;
        for (int rowTileIndex = 0; rowTileIndex < rowTiles; ++rowTileIndex)
        {
            auto rowIndex = rowTileIndex * tileSize;
            for (int columnTileIndex = 0; columnTileIndex < columnTiles; ++columnTileIndex)
            {
                auto columnIndex = columnTileIndex * tileSize;
                auto d = signal.GetSubMatrix(rowIndex, columnIndex, windowSize, windowSize);

                // Compute B'dB
                Multiply(Bt, d, Btd);
                Multiply(Btd, B, BtdB);

                // Elementwise multiply transformedFilter * BtdB into X
                auto& X = BtdB; // Rename to make code less opaque
                math::ElementwiseMultiplySet(transformedFilter, BtdB, X);

                // Now Compute result AtXA
                Multiply(At, X, AtX);
                Multiply(AtX, A, outputValues);
                auto outputTile = result.GetSubMatrix(rowIndex, columnIndex, tileSize, tileSize);
                outputTile.CopyFrom(outputValues);
            }
        }
    }

    //
    // Straightforward implementation of Winograd algorithm, using matrix multiplies for the data- and tile- transform steps
    //
    template <typename ValueType>
    void Convolve2DWinogradV1(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                              const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                              int numFilters,
                              int tileSize,
                              math::ChannelColumnRowTensor<ValueType>& result)
    {
        using Matrix = math::RowMatrix<ValueType>;

        const auto windowSize = static_cast<int>(transformedFilters.NumRows()) / numFilters; // == size of the transformed filters
        const auto filterSize = windowSize - tileSize + 1;
        const auto numChannels = static_cast<int>(signal.NumChannels());
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());
        assert(numFilters == static_cast<int>(result.NumChannels()));

        // Get transform matrices
        Matrix Bt = GetLeftDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix B = GetRightDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix At = GetLeftResultTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix A = GetRightResultTransformMatrix<ValueType>(tileSize, filterSize);

        // Temporary values
        Matrix Btd(windowSize, windowSize);
        Matrix AtX(tileSize, windowSize);
        Matrix outputValues(tileSize, tileSize);
        Matrix BtdB(windowSize, windowSize);
        Matrix signalSlice(signal.NumRows(), signal.NumColumns());
        Matrix transformedFilterSlice(windowSize, windowSize);

        int rowTiles = outputRows / tileSize;
        int columnTiles = outputColumns / tileSize;
        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            const auto filterOffset = filterIndex * windowSize;
            auto transformedFilter = transformedFilters.GetSubTensor(filterOffset, 0, 0, windowSize, windowSize, numChannels);
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                GetChannelSlice(transformedFilter, channelIndex, transformedFilterSlice);
                GetChannelSlice(signal, channelIndex, signalSlice);
                for (int rowTileIndex = 0; rowTileIndex < rowTiles; ++rowTileIndex)
                {
                    auto rowIndex = rowTileIndex * tileSize;
                    for (int columnTileIndex = 0; columnTileIndex < columnTiles; ++columnTileIndex)
                    {
                        auto columnIndex = columnTileIndex * tileSize;
                        auto d = signalSlice.GetSubMatrix(rowIndex, columnIndex, windowSize, windowSize);

                        // Compute B'dB
                        Multiply(Bt, d, Btd);
                        Multiply(Btd, B, BtdB);

                        // Elementwise multiply transformedFilter * BtdB into X
                        auto& X = BtdB; // Rename to make code less opaque
                        math::ElementwiseMultiplySet(transformedFilterSlice, BtdB, X);

                        // Now Compute result AtXA
                        Multiply(At, X, AtX);
                        Multiply(AtX, A, outputValues);

                        // Similarly to what we did with the input, get the appropriate part of the output,
                        // but append it into the result
                        AccumulateTile(outputValues, rowIndex, columnIndex, filterIndex, result);
                    }
                }
            }
        }
    }

    // result is a r x c x nf tensor
    template <typename ValueType>
    void Convolve2DWinogradV2(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                              const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                              int numFilters,
                              int tileSize,
                              int windowSize,
                              math::ChannelColumnRowTensor<ValueType>& result)
    {
        using Matrix = math::RowMatrix<ValueType>;
        using MatrixReference = math::RowMatrixReference<ValueType>;
        using Tensor = math::ChannelColumnRowTensor<ValueType>;

        // transformedFilters is a (wr*wc) x nf x d tensor
        const auto numChannels = static_cast<int>(signal.NumChannels());
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());
        const auto filterSize = windowSize - tileSize + 1;
        const auto numTileRows = outputRows / tileSize;
        const auto numTileColumns = outputColumns / tileSize;
        assert(numFilters == static_cast<int>(result.NumChannels()));
        assert(numFilters == static_cast<int>(transformedFilters.NumColumns()));
        assert(numChannels == static_cast<int>(transformedFilters.NumChannels()));

        // Get transform matrices
        Matrix Bt = GetLeftDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix B = GetRightDataTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix At = GetLeftResultTransformMatrix<ValueType>(tileSize, filterSize);
        Matrix A = GetRightResultTransformMatrix<ValueType>(tileSize, filterSize);

        // Temporary values
        Matrix Btd(windowSize, windowSize);
        Matrix AtX(tileSize, windowSize);
        Matrix outputValues(tileSize, tileSize);
        Matrix transformedInputWindow(windowSize, windowSize);
        Matrix transformedOutputWindow(windowSize, windowSize);
        Matrix outputTile(tileSize, tileSize);

        // transformedSignal is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
        Tensor transformedSignal(windowSize * windowSize, numChannels, numTileRows * numTileColumns); // 5D tensor: (windowSize * windowSize) x numChannels x (numTileRows * numTileColumns)

        // transformedOutput is (wr*wc) x nf x (tr * tc)
        Tensor transformedOutput(windowSize * windowSize, numFilters, numTileRows * numTileColumns);

        Tensor windowTensor(windowSize, windowSize, 1);
        MatrixReference windowMatrix(windowTensor.GetDataPointer(), windowSize, windowSize);

        // Note: these indices are iterating over input tiles, not pixels
        // TODO: make this a single loop over # of tiles, and convert that index to row/column indices on the fly
        for (int tileRowIndex = 0; tileRowIndex < numTileRows; ++tileRowIndex)
        {
            auto rowIndex = tileRowIndex * tileSize;
            for (int tileColumnIndex = 0; tileColumnIndex < numTileColumns; ++tileColumnIndex)
            {
                auto columnIndex = tileColumnIndex * tileSize;
                for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                {
                    auto inputWindow = signal.GetSubTensor(rowIndex, columnIndex, channelIndex, windowSize, windowSize, 1);
                    windowTensor.CopyFrom(inputWindow);

                    // Compute B'dB
                    Multiply(Bt, windowMatrix, Btd);
                    Multiply(Btd, B, transformedInputWindow);

                    // now splat transformedInputWindow into transformedSignal
                    SplatDataTile(transformedInputWindow, tileRowIndex, tileColumnIndex, channelIndex, numTileRows, numTileColumns, transformedSignal);
                }
            }
        }

        // Now do the multiply to reduce many entries in parallel
        //
        // transformedSignal is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
        // transformedFilters is a (wr*wc) x nf x d tensor
        // transformedOutput is (wr*wc) x nf x (tr * tc)
        // transformedFilters(i,j) * transformedSignal(i,j) -> transformedOutput  (nf x d) * (d x (tr*tc)) -> nf x (tr*tc)

        for (int windowRow = 0; windowRow < windowSize; ++windowRow)
        {
            for (int windowColumn = 0; windowColumn < windowSize; ++windowColumn)
            {
                // Get windowRow, windowColumn slice (reference) from transformedFilters, transformedSignal, and transformedOutput (as matrices)
                const auto windowPosition = (windowRow * windowSize) + windowColumn;

                // Note: we're transposing these tensor slices to get them in the correct order
                auto transformedFiltersSlice = transformedFilters.GetPrimarySlice(windowPosition).Transpose(); // nf x d   -- U in the paper
                auto transformedSignalSlice = transformedSignal.GetPrimarySlice(windowPosition).Transpose(); // d x (tr*tc)  -- V in the Lavin & Gray paper
                auto transformedOutputSlice = transformedOutput.GetPrimarySlice(windowPosition).Transpose(); // nf x (tr*tc)   -- M in the paper
                assert(static_cast<int>(transformedFiltersSlice.NumRows()) == numFilters && static_cast<int>(transformedFiltersSlice.NumColumns()) == numChannels);
                assert(static_cast<int>(transformedSignalSlice.NumRows()) == numChannels && static_cast<int>(transformedSignalSlice.NumColumns()) == (numTileRows * numTileColumns));
                assert(static_cast<int>(transformedOutputSlice.NumRows()) == numFilters && static_cast<int>(transformedOutputSlice.NumColumns()) == (numTileRows * numTileColumns));

                // multiply directly into transformedOutput
                Multiply(transformedFiltersSlice, transformedSignalSlice, transformedOutputSlice);
            }
        }

        // Now un-transform the result, copying tiles into the output
        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            // TODO: make this a single loop over # of tiles, and convert that index to row/column indices on the fly
            for (int tileRowIndex = 0; tileRowIndex < numTileRows; ++tileRowIndex)
            {
                for (int tileColumnIndex = 0; tileColumnIndex < numTileColumns; ++tileColumnIndex)
                {
                    // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
                    // transformedOutput is (wr*wc) x nf x (tr * tc), where tr == # tile rows and tc == # tile columns
                    // transformedOutputWindow is (wr*wc)
                    GetWindowSlice(transformedOutput, tileRowIndex, tileColumnIndex, filterIndex, numTileRows, numTileColumns, transformedOutputWindow);

                    // Now un-transform window into a tile
                    // outputTile is (tileSize x tileSize)
                    Multiply(At, transformedOutputWindow, AtX);
                    Multiply(AtX, A, outputTile);

                    // Copy into result
                    SplatOutputTile(outputTile, tileRowIndex, tileColumnIndex, filterIndex, tileSize, result);
                }
            }
        }
    }

    //
    // More efficient version of Winograd algorithm with data- and result- transform matrix calculations unrolled
    // The parts inside the inner loop were generated by the `winograd.py` script.
    //
    template <typename ValueType>
    void Convolve2DWinogradFast(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& transformedFilter, int tileSize, math::RowMatrix<ValueType>& result)
    {
        const auto windowSize = static_cast<int>(transformedFilter.NumRows());
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());

        // Temporary values
        math::RowMatrix<ValueType> X(windowSize, windowSize);

        // Testing tiling
        int tilesPerBigTile = 2; // process groups of 'tilesPerBigTile' x 'tilesPerBigTile' tiles at a time
        auto tileRows = outputRows / tileSize;
        auto tileColumns = outputColumns / tileSize;
        auto bigTileRows = tileRows / tilesPerBigTile;
        auto bigTileColumns = tileColumns / tilesPerBigTile;

        // First iterate over big tile indices
        for (int bigRowIndex = 0; bigRowIndex < bigTileRows; ++bigRowIndex)
        {
            for (int bigColumnIndex = 0; bigColumnIndex < bigTileColumns; ++bigColumnIndex)
            {
                // now iterate over subtiles within the big tile
                for (int tileRowIndex = 0; tileRowIndex < tilesPerBigTile; ++tileRowIndex)
                {
                    for (int tileColumnIndex = 0; tileColumnIndex < tilesPerBigTile; ++tileColumnIndex)
                    {
                        auto rowIndex = bigRowIndex * tilesPerBigTile * tileSize + tileRowIndex * tileSize;
                        auto columnIndex = bigColumnIndex * tilesPerBigTile * tileSize + tileColumnIndex * tileSize;
                        auto d = signal.GetSubMatrix(rowIndex, columnIndex, windowSize, windowSize);

                        if (tileSize == 2)
                        {
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

                            math::ElementwiseMultiplySet(transformedFilter, X, X);

                            // Now Compute result tile At * X * A
                            result(rowIndex, columnIndex) = ((((X(0, 0) + X(1, 0)) + X(2, 0)) + ((X(0, 1) + X(1, 1)) + X(2, 1))) + ((X(0, 2) + X(1, 2)) + X(2, 2)));
                            result(rowIndex, columnIndex + 1) = ((((X(0, 1) + X(1, 1)) + X(2, 1)) - ((X(0, 2) + X(1, 2)) + X(2, 2))) - ((X(0, 3) + X(1, 3)) + X(2, 3)));
                            result(rowIndex + 1, columnIndex) = ((((X(1, 0) - X(2, 0)) - X(3, 0)) + ((X(1, 1) - X(2, 1)) - X(3, 1))) + ((X(1, 2) - X(2, 2)) - X(3, 2)));
                            result(rowIndex + 1, columnIndex + 1) = ((((X(1, 1) - X(2, 1)) - X(3, 1)) - ((X(1, 2) - X(2, 2)) - X(3, 2))) - ((X(1, 3) - X(2, 3)) - X(3, 3)));
                        }
                        else
                        {
                            X(0, 0) = ((((((4 * d(0, 0)) + (-5 * d(2, 0))) + d(4, 0)) * 4) + ((((4 * d(0, 2)) + (-5 * d(2, 2))) + d(4, 2)) * -5)) + (((4 * d(0, 4)) + (-5 * d(2, 4))) + d(4, 4)));
                            X(0, 1) = (((((((4 * d(0, 1)) + (-5 * d(2, 1))) + d(4, 1)) * -4) + ((((4 * d(0, 2)) + (-5 * d(2, 2))) + d(4, 2)) * -4)) + (((4 * d(0, 3)) + (-5 * d(2, 3))) + d(4, 3))) + (((4 * d(0, 4)) + (-5 * d(2, 4))) + d(4, 4)));
                            X(0, 2) = (((((((4 * d(0, 1)) + (-5 * d(2, 1))) + d(4, 1)) * 4) + ((((4 * d(0, 2)) + (-5 * d(2, 2))) + d(4, 2)) * -4)) - (((4 * d(0, 3)) + (-5 * d(2, 3))) + d(4, 3))) + (((4 * d(0, 4)) + (-5 * d(2, 4))) + d(4, 4)));
                            X(0, 3) = (((((((4 * d(0, 1)) + (-5 * d(2, 1))) + d(4, 1)) * -2) - (((4 * d(0, 2)) + (-5 * d(2, 2))) + d(4, 2))) + ((((4 * d(0, 3)) + (-5 * d(2, 3))) + d(4, 3)) * 2)) + (((4 * d(0, 4)) + (-5 * d(2, 4))) + d(4, 4)));
                            X(0, 4) = (((((((4 * d(0, 1)) + (-5 * d(2, 1))) + d(4, 1)) * 2) - (((4 * d(0, 2)) + (-5 * d(2, 2))) + d(4, 2))) + ((((4 * d(0, 3)) + (-5 * d(2, 3))) + d(4, 3)) * -2)) + (((4 * d(0, 4)) + (-5 * d(2, 4))) + d(4, 4)));
                            X(0, 5) = ((((((4 * d(0, 1)) + (-5 * d(2, 1))) + d(4, 1)) * 4) + ((((4 * d(0, 3)) + (-5 * d(2, 3))) + d(4, 3)) * -5)) + (((4 * d(0, 5)) + (-5 * d(2, 5))) + d(4, 5)));
                            X(1, 0) = (((((((-4 * d(1, 0)) + (-4 * d(2, 0))) + d(3, 0)) + d(4, 0)) * 4) + (((((-4 * d(1, 2)) + (-4 * d(2, 2))) + d(3, 2)) + d(4, 2)) * -5)) + ((((-4 * d(1, 4)) + (-4 * d(2, 4))) + d(3, 4)) + d(4, 4)));
                            X(1, 1) = ((((((((-4 * d(1, 1)) + (-4 * d(2, 1))) + d(3, 1)) + d(4, 1)) * -4) + (((((-4 * d(1, 2)) + (-4 * d(2, 2))) + d(3, 2)) + d(4, 2)) * -4)) + ((((-4 * d(1, 3)) + (-4 * d(2, 3))) + d(3, 3)) + d(4, 3))) + ((((-4 * d(1, 4)) + (-4 * d(2, 4))) + d(3, 4)) + d(4, 4)));
                            X(1, 2) = ((((((((-4 * d(1, 1)) + (-4 * d(2, 1))) + d(3, 1)) + d(4, 1)) * 4) + (((((-4 * d(1, 2)) + (-4 * d(2, 2))) + d(3, 2)) + d(4, 2)) * -4)) - ((((-4 * d(1, 3)) + (-4 * d(2, 3))) + d(3, 3)) + d(4, 3))) + ((((-4 * d(1, 4)) + (-4 * d(2, 4))) + d(3, 4)) + d(4, 4)));
                            X(1, 3) = ((((((((-4 * d(1, 1)) + (-4 * d(2, 1))) + d(3, 1)) + d(4, 1)) * -2) - ((((-4 * d(1, 2)) + (-4 * d(2, 2))) + d(3, 2)) + d(4, 2))) + (((((-4 * d(1, 3)) + (-4 * d(2, 3))) + d(3, 3)) + d(4, 3)) * 2)) + ((((-4 * d(1, 4)) + (-4 * d(2, 4))) + d(3, 4)) + d(4, 4)));
                            X(1, 4) = ((((((((-4 * d(1, 1)) + (-4 * d(2, 1))) + d(3, 1)) + d(4, 1)) * 2) - ((((-4 * d(1, 2)) + (-4 * d(2, 2))) + d(3, 2)) + d(4, 2))) + (((((-4 * d(1, 3)) + (-4 * d(2, 3))) + d(3, 3)) + d(4, 3)) * -2)) + ((((-4 * d(1, 4)) + (-4 * d(2, 4))) + d(3, 4)) + d(4, 4)));
                            X(1, 5) = (((((((-4 * d(1, 1)) + (-4 * d(2, 1))) + d(3, 1)) + d(4, 1)) * 4) + (((((-4 * d(1, 3)) + (-4 * d(2, 3))) + d(3, 3)) + d(4, 3)) * -5)) + ((((-4 * d(1, 5)) + (-4 * d(2, 5))) + d(3, 5)) + d(4, 5)));
                            X(2, 0) = (((((((4 * d(1, 0)) + (-4 * d(2, 0))) - d(3, 0)) + d(4, 0)) * 4) + (((((4 * d(1, 2)) + (-4 * d(2, 2))) - d(3, 2)) + d(4, 2)) * -5)) + ((((4 * d(1, 4)) + (-4 * d(2, 4))) - d(3, 4)) + d(4, 4)));
                            X(2, 1) = ((((((((4 * d(1, 1)) + (-4 * d(2, 1))) - d(3, 1)) + d(4, 1)) * -4) + (((((4 * d(1, 2)) + (-4 * d(2, 2))) - d(3, 2)) + d(4, 2)) * -4)) + ((((4 * d(1, 3)) + (-4 * d(2, 3))) - d(3, 3)) + d(4, 3))) + ((((4 * d(1, 4)) + (-4 * d(2, 4))) - d(3, 4)) + d(4, 4)));
                            X(2, 2) = ((((((((4 * d(1, 1)) + (-4 * d(2, 1))) - d(3, 1)) + d(4, 1)) * 4) + (((((4 * d(1, 2)) + (-4 * d(2, 2))) - d(3, 2)) + d(4, 2)) * -4)) - ((((4 * d(1, 3)) + (-4 * d(2, 3))) - d(3, 3)) + d(4, 3))) + ((((4 * d(1, 4)) + (-4 * d(2, 4))) - d(3, 4)) + d(4, 4)));
                            X(2, 3) = ((((((((4 * d(1, 1)) + (-4 * d(2, 1))) - d(3, 1)) + d(4, 1)) * -2) - ((((4 * d(1, 2)) + (-4 * d(2, 2))) - d(3, 2)) + d(4, 2))) + (((((4 * d(1, 3)) + (-4 * d(2, 3))) - d(3, 3)) + d(4, 3)) * 2)) + ((((4 * d(1, 4)) + (-4 * d(2, 4))) - d(3, 4)) + d(4, 4)));
                            X(2, 4) = ((((((((4 * d(1, 1)) + (-4 * d(2, 1))) - d(3, 1)) + d(4, 1)) * 2) - ((((4 * d(1, 2)) + (-4 * d(2, 2))) - d(3, 2)) + d(4, 2))) + (((((4 * d(1, 3)) + (-4 * d(2, 3))) - d(3, 3)) + d(4, 3)) * -2)) + ((((4 * d(1, 4)) + (-4 * d(2, 4))) - d(3, 4)) + d(4, 4)));
                            X(2, 5) = (((((((4 * d(1, 1)) + (-4 * d(2, 1))) - d(3, 1)) + d(4, 1)) * 4) + (((((4 * d(1, 3)) + (-4 * d(2, 3))) - d(3, 3)) + d(4, 3)) * -5)) + ((((4 * d(1, 5)) + (-4 * d(2, 5))) - d(3, 5)) + d(4, 5)));
                            X(3, 0) = (((((((-2 * d(1, 0)) - d(2, 0)) + (2 * d(3, 0))) + d(4, 0)) * 4) + (((((-2 * d(1, 2)) - d(2, 2)) + (2 * d(3, 2))) + d(4, 2)) * -5)) + ((((-2 * d(1, 4)) - d(2, 4)) + (2 * d(3, 4))) + d(4, 4)));
                            X(3, 1) = ((((((((-2 * d(1, 1)) - d(2, 1)) + (2 * d(3, 1))) + d(4, 1)) * -4) + (((((-2 * d(1, 2)) - d(2, 2)) + (2 * d(3, 2))) + d(4, 2)) * -4)) + ((((-2 * d(1, 3)) - d(2, 3)) + (2 * d(3, 3))) + d(4, 3))) + ((((-2 * d(1, 4)) - d(2, 4)) + (2 * d(3, 4))) + d(4, 4)));
                            X(3, 2) = ((((((((-2 * d(1, 1)) - d(2, 1)) + (2 * d(3, 1))) + d(4, 1)) * 4) + (((((-2 * d(1, 2)) - d(2, 2)) + (2 * d(3, 2))) + d(4, 2)) * -4)) - ((((-2 * d(1, 3)) - d(2, 3)) + (2 * d(3, 3))) + d(4, 3))) + ((((-2 * d(1, 4)) - d(2, 4)) + (2 * d(3, 4))) + d(4, 4)));
                            X(3, 3) = ((((((((-2 * d(1, 1)) - d(2, 1)) + (2 * d(3, 1))) + d(4, 1)) * -2) - ((((-2 * d(1, 2)) - d(2, 2)) + (2 * d(3, 2))) + d(4, 2))) + (((((-2 * d(1, 3)) - d(2, 3)) + (2 * d(3, 3))) + d(4, 3)) * 2)) + ((((-2 * d(1, 4)) - d(2, 4)) + (2 * d(3, 4))) + d(4, 4)));
                            X(3, 4) = ((((((((-2 * d(1, 1)) - d(2, 1)) + (2 * d(3, 1))) + d(4, 1)) * 2) - ((((-2 * d(1, 2)) - d(2, 2)) + (2 * d(3, 2))) + d(4, 2))) + (((((-2 * d(1, 3)) - d(2, 3)) + (2 * d(3, 3))) + d(4, 3)) * -2)) + ((((-2 * d(1, 4)) - d(2, 4)) + (2 * d(3, 4))) + d(4, 4)));
                            X(3, 5) = (((((((-2 * d(1, 1)) - d(2, 1)) + (2 * d(3, 1))) + d(4, 1)) * 4) + (((((-2 * d(1, 3)) - d(2, 3)) + (2 * d(3, 3))) + d(4, 3)) * -5)) + ((((-2 * d(1, 5)) - d(2, 5)) + (2 * d(3, 5))) + d(4, 5)));
                            X(4, 0) = (((((((2 * d(1, 0)) - d(2, 0)) + (-2 * d(3, 0))) + d(4, 0)) * 4) + (((((2 * d(1, 2)) - d(2, 2)) + (-2 * d(3, 2))) + d(4, 2)) * -5)) + ((((2 * d(1, 4)) - d(2, 4)) + (-2 * d(3, 4))) + d(4, 4)));
                            X(4, 1) = ((((((((2 * d(1, 1)) - d(2, 1)) + (-2 * d(3, 1))) + d(4, 1)) * -4) + (((((2 * d(1, 2)) - d(2, 2)) + (-2 * d(3, 2))) + d(4, 2)) * -4)) + ((((2 * d(1, 3)) - d(2, 3)) + (-2 * d(3, 3))) + d(4, 3))) + ((((2 * d(1, 4)) - d(2, 4)) + (-2 * d(3, 4))) + d(4, 4)));
                            X(4, 2) = ((((((((2 * d(1, 1)) - d(2, 1)) + (-2 * d(3, 1))) + d(4, 1)) * 4) + (((((2 * d(1, 2)) - d(2, 2)) + (-2 * d(3, 2))) + d(4, 2)) * -4)) - ((((2 * d(1, 3)) - d(2, 3)) + (-2 * d(3, 3))) + d(4, 3))) + ((((2 * d(1, 4)) - d(2, 4)) + (-2 * d(3, 4))) + d(4, 4)));
                            X(4, 3) = ((((((((2 * d(1, 1)) - d(2, 1)) + (-2 * d(3, 1))) + d(4, 1)) * -2) - ((((2 * d(1, 2)) - d(2, 2)) + (-2 * d(3, 2))) + d(4, 2))) + (((((2 * d(1, 3)) - d(2, 3)) + (-2 * d(3, 3))) + d(4, 3)) * 2)) + ((((2 * d(1, 4)) - d(2, 4)) + (-2 * d(3, 4))) + d(4, 4)));
                            X(4, 4) = ((((((((2 * d(1, 1)) - d(2, 1)) + (-2 * d(3, 1))) + d(4, 1)) * 2) - ((((2 * d(1, 2)) - d(2, 2)) + (-2 * d(3, 2))) + d(4, 2))) + (((((2 * d(1, 3)) - d(2, 3)) + (-2 * d(3, 3))) + d(4, 3)) * -2)) + ((((2 * d(1, 4)) - d(2, 4)) + (-2 * d(3, 4))) + d(4, 4)));
                            X(4, 5) = (((((((2 * d(1, 1)) - d(2, 1)) + (-2 * d(3, 1))) + d(4, 1)) * 4) + (((((2 * d(1, 3)) - d(2, 3)) + (-2 * d(3, 3))) + d(4, 3)) * -5)) + ((((2 * d(1, 5)) - d(2, 5)) + (-2 * d(3, 5))) + d(4, 5)));
                            X(5, 0) = ((((((4 * d(1, 0)) + (-5 * d(3, 0))) + d(5, 0)) * 4) + ((((4 * d(1, 2)) + (-5 * d(3, 2))) + d(5, 2)) * -5)) + (((4 * d(1, 4)) + (-5 * d(3, 4))) + d(5, 4)));
                            X(5, 1) = (((((((4 * d(1, 1)) + (-5 * d(3, 1))) + d(5, 1)) * -4) + ((((4 * d(1, 2)) + (-5 * d(3, 2))) + d(5, 2)) * -4)) + (((4 * d(1, 3)) + (-5 * d(3, 3))) + d(5, 3))) + (((4 * d(1, 4)) + (-5 * d(3, 4))) + d(5, 4)));
                            X(5, 2) = (((((((4 * d(1, 1)) + (-5 * d(3, 1))) + d(5, 1)) * 4) + ((((4 * d(1, 2)) + (-5 * d(3, 2))) + d(5, 2)) * -4)) - (((4 * d(1, 3)) + (-5 * d(3, 3))) + d(5, 3))) + (((4 * d(1, 4)) + (-5 * d(3, 4))) + d(5, 4)));
                            X(5, 3) = (((((((4 * d(1, 1)) + (-5 * d(3, 1))) + d(5, 1)) * -2) - (((4 * d(1, 2)) + (-5 * d(3, 2))) + d(5, 2))) + ((((4 * d(1, 3)) + (-5 * d(3, 3))) + d(5, 3)) * 2)) + (((4 * d(1, 4)) + (-5 * d(3, 4))) + d(5, 4)));
                            X(5, 4) = (((((((4 * d(1, 1)) + (-5 * d(3, 1))) + d(5, 1)) * 2) - (((4 * d(1, 2)) + (-5 * d(3, 2))) + d(5, 2))) + ((((4 * d(1, 3)) + (-5 * d(3, 3))) + d(5, 3)) * -2)) + (((4 * d(1, 4)) + (-5 * d(3, 4))) + d(5, 4)));
                            X(5, 5) = ((((((4 * d(1, 1)) + (-5 * d(3, 1))) + d(5, 1)) * 4) + ((((4 * d(1, 3)) + (-5 * d(3, 3))) + d(5, 3)) * -5)) + (((4 * d(1, 5)) + (-5 * d(3, 5))) + d(5, 5)));

                            math::ElementwiseMultiplySet(transformedFilter, X, X);

                            result(rowIndex, columnIndex) = ((((((((X(0, 0) + X(1, 0)) + X(2, 0)) + X(3, 0)) + X(4, 0)) + ((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1))) + ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + ((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3))) + ((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)));
                            result(rowIndex, columnIndex + 1) = (((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) - ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 2)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * -2));
                            result(rowIndex, columnIndex + 2) = (((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) + ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 4)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * 4));
                            result(rowIndex, columnIndex + 3) = ((((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) - ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 8)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * -8)) + ((((X(0, 5) + X(1, 5)) + X(2, 5)) + X(3, 5)) + X(4, 5)));
                            result(rowIndex + 1, columnIndex) = (((((((X(1, 0) - X(2, 0)) + (2 * X(3, 0))) + (-2 * X(4, 0))) + (((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1)))) + (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + (((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3)))) + (((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))));
                            result(rowIndex + 1, columnIndex + 1) = ((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) - (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 2)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * -2));
                            result(rowIndex + 1, columnIndex + 2) = ((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) + (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 4)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * 4));
                            result(rowIndex + 1, columnIndex + 3) = (((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) - (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 8)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * -8)) + (((X(1, 5) - X(2, 5)) + (2 * X(3, 5))) + (-2 * X(4, 5))));
                            result(rowIndex + 2, columnIndex) = (((((((X(1, 0) + X(2, 0)) + (4 * X(3, 0))) + (4 * X(4, 0))) + (((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1)))) + (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + (((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3)))) + (((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))));
                            result(rowIndex + 2, columnIndex + 1) = ((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) - (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 2)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * -2));
                            result(rowIndex + 2, columnIndex + 2) = ((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) + (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 4)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * 4));
                            result(rowIndex + 2, columnIndex + 3) = (((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) - (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 8)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * -8)) + (((X(1, 5) + X(2, 5)) + (4 * X(3, 5))) + (4 * X(4, 5))));
                            result(rowIndex + 3, columnIndex) = ((((((((X(1, 0) - X(2, 0)) + (8 * X(3, 0))) + (-8 * X(4, 0))) + X(5, 0)) + ((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1))) + ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + ((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3))) + ((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)));
                            result(rowIndex + 3, columnIndex + 1) = (((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) - ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 2)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * -2));
                            result(rowIndex + 3, columnIndex + 2) = (((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) + ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 4)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * 4));
                            result(rowIndex + 3, columnIndex + 3) = ((((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) - ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 8)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * -8)) + ((((X(1, 5) - X(2, 5)) + (8 * X(3, 5))) + (-8 * X(4, 5))) + X(5, 5)));
                        }
                    }
                }
            }
        }
    }
}
}
