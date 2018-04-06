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
#include <array>
#include <cassert>
#include <initializer_list>

namespace ell
{
namespace dsp
{
    namespace
    {
        //
        // Helper function to avoid annoying double-to-float errors when creating matrices from literals
        //
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

        //
        // Helper function for basic matrix multiplication
        //
        template <typename ValueType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::MatrixLayout layout3>
        void Multiply(const math::ConstMatrixReference<ValueType, layout1>& A, const math::ConstMatrixReference<ValueType, layout2>& B, math::MatrixReference<ValueType, layout3>& C)
        {
            math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), A, B, static_cast<ValueType>(0.0), C);
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
        void SplatTransformedInputTile(const math::RowMatrix<ValueType>& dataTile, int tileRowIndex, int tileColumnIndex, int channelIndex, int numTileRows, int numTileColumns, math::ChannelColumnRowTensorReference<ValueType>& transformedSignal)
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
    }

    //
    // Explicit instantiations --- placed at the top of the file for readability
    //

    // Basic 1D entry points
    template math::RowVector<float> Convolve1DWinograd(const math::RowVector<float>& signal, const math::RowVector<float>& filter);
    template math::RowVector<float> Convolve1DWinograd(const math::RowVector<float>& signal, const math::RowVector<float>& filter, int tileSize);
    template math::RowVector<double> Convolve1DWinograd(const math::RowVector<double>& signal, const math::RowVector<double>& filter);
    template math::RowVector<double> Convolve1DWinograd(const math::RowVector<double>& signal, const math::RowVector<double>& filter, int tileSize);

    // Basic tensor-valued 2D entry points
    template math::ChannelColumnRowTensor<float> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<float>& signal, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, WinogradFilterOrder order);
    template math::ChannelColumnRowTensor<float> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<float>& signal, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, int tileSize, WinogradFilterOrder order);
    template math::ChannelColumnRowTensor<float> Convolve2DWinogradPretransformed(const math::ConstChannelColumnRowTensorReference<float>& signal, const math::ConstChannelColumnRowTensorReference<float>& transformedFilters, int numFilters, int tileSize, int filterSize, WinogradFilterOrder order);
    template math::ChannelColumnRowTensor<double> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<double>& signal, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, WinogradFilterOrder order);
    template math::ChannelColumnRowTensor<double> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<double>& signal, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, int tileSize, WinogradFilterOrder order);
    template math::ChannelColumnRowTensor<double> Convolve2DWinogradPretransformed(const math::ConstChannelColumnRowTensorReference<double>& signal, const math::ConstChannelColumnRowTensorReference<double>& transformedFilters, int numFilters, int tileSize, int filterSize, WinogradFilterOrder order);

    // Winograd implementation functions
    template math::RowMatrix<float> GetLeftDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetLeftFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetLeftResultTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<float> GetRightResultTransformMatrix(int tileSize, int filterSize);
    template math::ChannelColumnRowTensor<float> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<float> filters, int numFilters, int tileSize, WinogradFilterOrder order);
    template void TransformFilters(math::ConstChannelColumnRowTensorReference<float> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<float> transformedFilters);
    template math::RowMatrix<double> GetLeftDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightDataTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetLeftFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightFilterTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetLeftResultTransformMatrix(int tileSize, int filterSize);
    template math::RowMatrix<double> GetRightResultTransformMatrix(int tileSize, int filterSize);
    template math::ChannelColumnRowTensor<double> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<double> filters, int numFilters, int tileSize, WinogradFilterOrder order);
    template void TransformFilters(math::ConstChannelColumnRowTensorReference<double> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<double> transformedFilters);

    //
    // Declarations of implementation functions local to this file
    //

    // 2D
    template <typename ValueType>
    void Convolve2DWinogradFiltersFirstMatrix(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int filterSize, math::ChannelColumnRowTensor<ValueType>& result);

    template <typename ValueType>
    void Convolve2DWinogradFiltersFirst(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int filterSize, math::ChannelColumnRowTensor<ValueType>& result);

    template <typename ValueType>
    void Convolve2DWinogradTilesFirst(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int filterSize, math::ChannelColumnRowTensor<ValueType>& result);

    //
    // Winograd matrix functions
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
    math::ChannelColumnRowTensor<ValueType> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<ValueType> filters, int numFilters, int tileSize, WinogradFilterOrder order)
    {
        // Input filters tensor is (nf*fr) x fc x d
        using Matrix = math::RowMatrix<ValueType>;
        using Tensor = math::ChannelColumnRowTensor<ValueType>;
        const auto filterSize = static_cast<int>(filters.NumRows()) / numFilters;
        const auto windowSize = tileSize + filterSize - 1;
        const auto numChannels = static_cast<int>(filters.NumChannels());

        // The two algorithm versions use different orderings of the transformed filter tensor
        if (order == WinogradFilterOrder::filtersFirst)
        {
            // In "filtersFirst", filters are in nf x wr x wc x d order  (where wr == wc == windowSize)
            // They're represented as a (nf * wr) x wc x d tensor

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
        else if (order == WinogradFilterOrder::tilesFirst)
        {
            // In "tilesFirst", filters are in wr x wc x nf x d order  (where wr == wc == windowSize)
            // They're represented as a (wr * wc) x nf x d tensor
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
    // 1D Winograd convolution implementation
    //

    // Y = A' * (Gg .* B'd)
    //
    // g = filter ([g0 g1 g2]')
    // d = signal ([d0 d1 d2 d3]')
    //

    //
    // Classes used for implementing 1D Winograd convolution for sizes known at compile time
    //

    template <typename ValueType, int tileSize, int filterSize>
    struct FixedWinograd1D;

    template <typename ValueType>
    struct FixedWinograd1D<ValueType, 2, 3>
    {
        static constexpr int tileSize = 2;
        static constexpr int filterSize = 3;

        static void Convolve(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, math::RowVector<ValueType>& result)
        {
            assert(filter.Size() == filterSize);
            const int outputSize = static_cast<int>(result.Size());

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
            int numFullTiles = outputSize / tileSize;
            for (int tileIndex = 0; tileIndex < numFullTiles; ++tileIndex)
            {
                const int index = tileIndex * tileSize;
                ValueType d2 = d[index + 2];
                ValueType d3 = d[index + 3];

                // elementwise vector multiply
                const auto m1 = (d0 - d2) * Gg0;
                const auto m2 = (d1 + d2) * Gg1;
                const auto m3 = (d2 - d1) * Gg2;
                const auto m4 = (d1 - d3) * Gg3;
                result[index] = m1 + m2 + m3;
                result[index + 1] = m2 - m3 - m4;

                // shift d0, d1
                d0 = d2;
                d1 = d3;
            }

            // If the last tile is only partially full, compute it here
            auto remainder = outputSize % tileSize;
            if (remainder > 0)
            {
                assert(remainder == 1); // with tileSize == 2, the only possible remainder is '1'
                const int tileIndex = numFullTiles; // the last tile
                const int index = tileIndex * tileSize;
                ValueType d2 = d[index + 2];

                // elementwise vector multiply
                auto m1 = (d0 - d2) * Gg0;
                auto m2 = (d1 + d2) * Gg1;
                auto m3 = (d2 - d1) * Gg2;
                result[index] = m1 + m2 + m3;
            }
        }
    };

    //
    // Actual API function implementation
    //
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter)
    {
        const int tileSize = 2;
        return Convolve1DWinograd(signal, filter, tileSize);
    }

    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter, int tileSize)
    {
        using namespace std::string_literals;
        const int filterSize = static_cast<int>(filter.Size());
        const int outputSize = static_cast<int>(signal.Size()) - filterSize + 1;
        math::RowVector<ValueType> result(outputSize);
        if (tileSize == 2 && filterSize == 3)
        {
            FixedWinograd1D<ValueType, 2, 3>::Convolve(signal, filter, result);
        }
        else
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "1D Winograd convolution not implemented for tile size "s + std::to_string(tileSize) + " and filter size " + std::to_string(filterSize));
        }
        return result;
    }

    //
    // 2D convolution
    //

    // 2D array class for fixed-size arrays

    template <typename ValueType, int rows, int columns>
    class Fixed2DArray
    {
    public:
        constexpr Fixed2DArray() : _data({0}) {}

        void CopyFrom(const math::ConstChannelColumnRowTensorReference<ValueType>& data)
        {
            const auto dataPtr = data.GetConstDataPointer();
            if (data.IsContiguous())
            {
                std::copy(dataPtr, dataPtr + (rows * columns), _data.data());
            }
            else if (data.GetIncrement1() == 1)
            {
                auto stride = data.GetIncrement2();
                for (int rowIndex = 0; rowIndex < rows; ++rowIndex)
                {
                    std::copy(dataPtr + rowIndex * stride, dataPtr + (rowIndex)*stride + columns, _data.data() + rowIndex * columns);
                }
            }
            else
            {
                for (int rowIndex = 0; rowIndex < rows; ++rowIndex)
                {
                    for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
                    {
                        (*this)(rowIndex, columnIndex) = data(rowIndex, columnIndex, 0);
                    }
                }
            }
        }

        void CopyFrom(const ValueType* dataPtr, int startRow, int startColumn, int channelIndex, int increment1, int increment2)
        {
            CopyFrom(dataPtr, startRow, startColumn, channelIndex, rows, columns, increment1, increment2);
        }

        void CopyFrom(const ValueType* dataPtr, int startRow, int startColumn, int channelIndex, int numRows, int numColumns, int increment1, int increment2)
        {
            for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < numColumns; ++columnIndex)
                {
                    _data[rowIndex * columns + columnIndex] = dataPtr[(rowIndex + startRow) * increment2 + (columnIndex + startColumn) * increment1 + channelIndex];
                }
            }
        }

        ValueType operator()(int row, int column) const
        {
            return _data[row * columns + column];
        }

        ValueType& operator()(int row, int column)
        {
            return _data[row * columns + column];
        }

    private:
        std::array<ValueType, rows * columns> _data;
    };

    //
    // Helper class encapsulating Winograd transform matrix application
    //
    template <typename ValueType, int tileSize, int filterSize>
    struct FixedWinogradTransform2D;

    //
    // The code inside these functions was generated by the `winograd.py` script.
    //
    template <typename ValueType>
    struct FixedWinogradTransform2D<ValueType, 2, 3>
    {
        static constexpr int tileSize = 2;
        static constexpr int filterSize = 3;
        static constexpr auto windowSize = filterSize + tileSize - 1;

        template <typename MatrixType1, typename MatrixType2>
        static void TransformInputWindow(const MatrixType1& d, MatrixType2& X)
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
        }

        template <typename MatrixType1, typename MatrixType2>
        static void TransformOutputTile(const MatrixType1& X, MatrixType2& result)
        {
            result(0, 0) = ((((X(0, 0) + X(1, 0)) + X(2, 0)) + ((X(0, 1) + X(1, 1)) + X(2, 1))) + ((X(0, 2) + X(1, 2)) + X(2, 2)));
            result(0, 1) = ((((X(0, 1) + X(1, 1)) + X(2, 1)) - ((X(0, 2) + X(1, 2)) + X(2, 2))) - ((X(0, 3) + X(1, 3)) + X(2, 3)));
            result(1, 0) = ((((X(1, 0) - X(2, 0)) - X(3, 0)) + ((X(1, 1) - X(2, 1)) - X(3, 1))) + ((X(1, 2) - X(2, 2)) - X(3, 2)));
            result(1, 1) = ((((X(1, 1) - X(2, 1)) - X(3, 1)) - ((X(1, 2) - X(2, 2)) - X(3, 2))) - ((X(1, 3) - X(2, 3)) - X(3, 3)));
        }
    };

    //
    // The code inside these functions was generated by the `winograd.py` script.
    //
    template <typename ValueType>
    struct FixedWinogradTransform2D<ValueType, 4, 3>
    {
        static constexpr int tileSize = 4;
        static constexpr int filterSize = 3;
        static constexpr auto windowSize = filterSize + tileSize - 1;

        using TileArray = Fixed2DArray<ValueType, tileSize, tileSize>;
        using WindowArray = Fixed2DArray<ValueType, windowSize, windowSize>;

        template <typename MatrixType1, typename MatrixType2>
        static void TransformInputWindow(const MatrixType1& d, MatrixType2& X)
        {
            // Compute B'dB
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
        }

        template <typename MatrixType1, typename MatrixType2>
        static void TransformOutputTile(const MatrixType1& X, MatrixType2& result)
        {
            result(0, 0) = ((((((((X(0, 0) + X(1, 0)) + X(2, 0)) + X(3, 0)) + X(4, 0)) + ((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1))) + ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + ((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3))) + ((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)));
            result(0, 1) = (((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) - ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 2)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * -2));
            result(0, 2) = (((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) + ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 4)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * 4));
            result(0, 3) = ((((((((X(0, 1) + X(1, 1)) + X(2, 1)) + X(3, 1)) + X(4, 1)) - ((((X(0, 2) + X(1, 2)) + X(2, 2)) + X(3, 2)) + X(4, 2))) + (((((X(0, 3) + X(1, 3)) + X(2, 3)) + X(3, 3)) + X(4, 3)) * 8)) + (((((X(0, 4) + X(1, 4)) + X(2, 4)) + X(3, 4)) + X(4, 4)) * -8)) + ((((X(0, 5) + X(1, 5)) + X(2, 5)) + X(3, 5)) + X(4, 5)));
            result(1, 0) = (((((((X(1, 0) - X(2, 0)) + (2 * X(3, 0))) + (-2 * X(4, 0))) + (((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1)))) + (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + (((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3)))) + (((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))));
            result(1, 1) = ((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) - (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 2)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * -2));
            result(1, 2) = ((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) + (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 4)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * 4));
            result(1, 3) = (((((((X(1, 1) - X(2, 1)) + (2 * X(3, 1))) + (-2 * X(4, 1))) - (((X(1, 2) - X(2, 2)) + (2 * X(3, 2))) + (-2 * X(4, 2)))) + ((((X(1, 3) - X(2, 3)) + (2 * X(3, 3))) + (-2 * X(4, 3))) * 8)) + ((((X(1, 4) - X(2, 4)) + (2 * X(3, 4))) + (-2 * X(4, 4))) * -8)) + (((X(1, 5) - X(2, 5)) + (2 * X(3, 5))) + (-2 * X(4, 5))));
            result(2, 0) = (((((((X(1, 0) + X(2, 0)) + (4 * X(3, 0))) + (4 * X(4, 0))) + (((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1)))) + (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + (((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3)))) + (((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))));
            result(2, 1) = ((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) - (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 2)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * -2));
            result(2, 2) = ((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) + (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 4)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * 4));
            result(2, 3) = (((((((X(1, 1) + X(2, 1)) + (4 * X(3, 1))) + (4 * X(4, 1))) - (((X(1, 2) + X(2, 2)) + (4 * X(3, 2))) + (4 * X(4, 2)))) + ((((X(1, 3) + X(2, 3)) + (4 * X(3, 3))) + (4 * X(4, 3))) * 8)) + ((((X(1, 4) + X(2, 4)) + (4 * X(3, 4))) + (4 * X(4, 4))) * -8)) + (((X(1, 5) + X(2, 5)) + (4 * X(3, 5))) + (4 * X(4, 5))));
            result(3, 0) = ((((((((X(1, 0) - X(2, 0)) + (8 * X(3, 0))) + (-8 * X(4, 0))) + X(5, 0)) + ((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1))) + ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + ((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3))) + ((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)));
            result(3, 1) = (((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) - ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 2)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * -2));
            result(3, 2) = (((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) + ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 4)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * 4));
            result(3, 3) = ((((((((X(1, 1) - X(2, 1)) + (8 * X(3, 1))) + (-8 * X(4, 1))) + X(5, 1)) - ((((X(1, 2) - X(2, 2)) + (8 * X(3, 2))) + (-8 * X(4, 2))) + X(5, 2))) + (((((X(1, 3) - X(2, 3)) + (8 * X(3, 3))) + (-8 * X(4, 3))) + X(5, 3)) * 8)) + (((((X(1, 4) - X(2, 4)) + (8 * X(3, 4))) + (-8 * X(4, 4))) + X(5, 4)) * -8)) + ((((X(1, 5) - X(2, 5)) + (8 * X(3, 5))) + (-8 * X(4, 5))) + X(5, 5)));
        }
    };

    //
    // Helper class to implement Winograd convolution steps
    //
    template <typename ValueType, int tileSizeValue, int filterSizeValue>
    struct FixedWinograd2D
    {
        static constexpr int tileSize = tileSizeValue;
        static constexpr int filterSize = filterSizeValue;
        static constexpr int windowSize = filterSize + tileSize - 1;

        using TileArray = Fixed2DArray<ValueType, tileSize, tileSize>;
        using WindowArray = Fixed2DArray<ValueType, windowSize, windowSize>;
        using Tensor = math::ChannelColumnRowTensor<ValueType>;
        using TensorReference = math::ChannelColumnRowTensorReference<ValueType>;
        using ConstTensorReference = math::ConstChannelColumnRowTensorReference<ValueType>;

        static void TransformInput(const ConstTensorReference& signal,
                                   int numOutputRows,
                                   int numOutputColumns,
                                   int numChannels,
                                   Tensor& transformedSignal)
        {
            const auto numFullTileRows = numOutputRows / tileSize;
            const auto numFullTileColumns = numOutputColumns / tileSize;
            const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
            const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;

            WindowArray d;
            WindowArray X;

            // Note: these indices are iterating over input tiles, not pixels

            //
            // First, visit all fully-covered input tiles
            //
            for (int tileRowIndex = 0; tileRowIndex < numFullTileRows; ++tileRowIndex)
            {
                for (int tileColumnIndex = 0; tileColumnIndex < numFullTileColumns; ++tileColumnIndex)
                {
                    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                    {
                        // Get the input window
                        GetInputWindow(signal, tileRowIndex, tileColumnIndex, channelIndex, d);

                        // Transform it
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformInputWindow(d, X);

                        // Now splat transformedInputWindow into transformedSignal
                        SplatTransformedInputTile(X, tileRowIndex, tileColumnIndex, channelIndex, numTileRows, numTileColumns, transformedSignal);
                    }
                }
            }

            //
            // Now go and fill in transformed data for tiles that aren't fully contained in the input image
            //

            // First, the bottom row
            const auto ws = windowSize; // STYLE: Necessary to prevent build error on clang
            if (numTileRows > numFullTileRows)
            {
                assert(numTileRows == numFullTileRows + 1);
                const int tileRowIndex = numFullTileRows;
                const auto startRowIndex = tileRowIndex * tileSize;
                const auto rows = std::min(ws, static_cast<int>(signal.NumRows() - startRowIndex));
                const auto columns = ws;
                for (int tileColumnIndex = 0; tileColumnIndex < numFullTileColumns; ++tileColumnIndex)
                {
                    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                    {
                        // Get the input window
                        GetPartialInputWindow(signal, tileRowIndex, tileColumnIndex, channelIndex, rows, columns, d);

                        // Transform it
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformInputWindow(d, X);

                        // Now splat transformedInputWindow into transformedSignal
                        SplatTransformedInputTile(X, tileRowIndex, tileColumnIndex, channelIndex, numTileRows, numTileColumns, transformedSignal);
                    }
                }
            }

            // Then the righthand column
            if (numTileColumns > numFullTileColumns)
            {
                assert(numTileColumns == numFullTileColumns + 1);
                const int tileColumnIndex = numFullTileColumns;
                const auto startColumnIndex = tileColumnIndex * tileSize;
                const auto rows = ws;
                const auto columns = std::min(ws, static_cast<int>(signal.NumColumns() - startColumnIndex));

                for (int tileRowIndex = 0; tileRowIndex < numFullTileRows; ++tileRowIndex)
                {
                    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                    {
                        // Get the input window
                        GetPartialInputWindow(signal, tileRowIndex, tileColumnIndex, channelIndex, rows, columns, d);

                        // Transform it
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformInputWindow(d, X);

                        // Now splat transformedInputWindow into transformedSignal
                        SplatTransformedInputTile(X, tileRowIndex, tileColumnIndex, channelIndex, numTileRows, numTileColumns, transformedSignal);
                    }
                }
            }

            // Finally, the lower-righthand corner
            if (numTileRows > numFullTileRows && numTileColumns > numFullTileColumns)
            {
                assert(numTileRows == numFullTileRows + 1);
                const int tileRowIndex = numFullTileRows;
                const int tileColumnIndex = numFullTileColumns;
                const auto startRowIndex = tileRowIndex * tileSize;
                const auto startColumnIndex = tileColumnIndex * tileSize;
                const auto rows = std::min(ws, static_cast<int>(signal.NumRows() - startRowIndex));
                const auto columns = std::min(ws, static_cast<int>(signal.NumColumns() - startColumnIndex));

                for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                {
                    // Get the input window
                    GetPartialInputWindow(signal, tileRowIndex, tileColumnIndex, channelIndex, rows, columns, d);

                    // Transform it
                    FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformInputWindow(d, X);

                    // Now splat transformedInputWindow into transformedSignal
                    SplatTransformedInputTile(X, tileRowIndex, tileColumnIndex, channelIndex, numTileRows, numTileColumns, transformedSignal);
                }
            }
        }

        static void GetInputWindow(const ConstTensorReference& signal,
                                   int tileRowIndex,
                                   int tileColumnIndex,
                                   int channelIndex,
                                   WindowArray& d)
        {
            auto rowIndex = tileRowIndex * tileSize;
            auto columnIndex = tileColumnIndex * tileSize;
            d.CopyFrom(signal.GetConstDataPointer(), rowIndex, columnIndex, channelIndex, static_cast<int>(signal.GetIncrement1()), static_cast<int>(signal.GetIncrement2()));
        }

        static void GetPartialInputWindow(const ConstTensorReference& signal,
                                          int tileRowIndex,
                                          int tileColumnIndex,
                                          int channelIndex,
                                          int rows,
                                          int columns,
                                          WindowArray& d)
        {
            auto rowIndex = tileRowIndex * tileSize;
            auto columnIndex = tileColumnIndex * tileSize;
            d.CopyFrom(signal.GetConstDataPointer(), rowIndex, columnIndex, channelIndex, rows, columns, static_cast<int>(signal.GetIncrement1()), static_cast<int>(signal.GetIncrement2()));
        }

        static void SplatTransformedInputTile(const WindowArray& dataTile, int tileRowIndex, int tileColumnIndex, int channelIndex, int numTileRows, int numTileColumns, TensorReference& transformedSignal)
        {
            UNUSED(numTileRows);

            // tr, tc are tile indices: r/tileSize and c/tileSize
            // dataTile is a wr x wc matrix
            // transformedSignal is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
            assert(static_cast<int>(transformedSignal.NumRows()) == windowSize * windowSize);

            auto transformedSignalPtr = transformedSignal.GetDataPointer();
            auto transformedSignalVector = math::RowVectorReference<ValueType>(transformedSignalPtr, transformedSignal.Size());
            const auto windowEntryStride = transformedSignal.GetIncrement2();
            const auto channelStride = transformedSignal.GetIncrement1();
            const auto tileOffset = (channelStride * channelIndex) + (tileRowIndex * numTileColumns) + tileColumnIndex;
            for (int windowRowIndex = 0; windowRowIndex < windowSize; ++windowRowIndex)
            {
                for (int windowColumnIndex = 0; windowColumnIndex < windowSize; ++windowColumnIndex)
                {
                    transformedSignalVector[(windowRowIndex * windowSize + windowColumnIndex) * windowEntryStride + tileOffset] = dataTile(windowRowIndex, windowColumnIndex);
                }
            }
        }

        static void GetTransformedOutputWindow(math::ConstChannelColumnRowTensorReference<ValueType> transformedOutput, int tileRowIndex, int tileColumnIndex, int filterIndex, int tileRows, int tileColumns, WindowArray& transformedOutputWindow)
        {
            UNUSED(tileRows);
            assert(filterIndex < static_cast<int>(transformedOutput.NumColumns()));

            const int tileIndex = tileRowIndex * tileColumns + tileColumnIndex;
            const int offset = filterIndex * static_cast<int>(transformedOutput.GetIncrement1()) + tileIndex;
            const auto dataPtr = transformedOutput.GetConstDataPointer();
            for (int windowRowIndex = 0; windowRowIndex < windowSize; ++windowRowIndex)
            {
                for (int windowColumnIndex = 0; windowColumnIndex < windowSize; ++windowColumnIndex)
                {
                    const int windowPos = (windowRowIndex * windowSize) + windowColumnIndex;
                    transformedOutputWindow(windowRowIndex, windowColumnIndex) = dataPtr[windowPos * transformedOutput.GetIncrement2() + offset];
                }
            }
        }

        // outputTile is a tr x tc matrix
        // result is a r x c x nf tensor
        static void SplatOutputTile(const TileArray& outputTile,
                                    int tileRowIndex,
                                    int tileColumnIndex,
                                    int filterIndex,
                                    math::ChannelColumnRowTensorReference<ValueType>& result)
        {
            // iterate over entries in the tile
            for (int rowIndex = 0; rowIndex < tileSize; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < tileSize; ++columnIndex)
                {
                    result((tileRowIndex * tileSize) + rowIndex, (tileColumnIndex * tileSize) + columnIndex, filterIndex) = outputTile(rowIndex, columnIndex);
                }
            }
        }

        // outputTile is a tr x tc matrix
        // result is a r x c x nf tensor
        static void SplatPartialOutputTile(const TileArray& outputTile,
                                           int tileRowIndex,
                                           int tileColumnIndex,
                                           int filterIndex,
                                           int rows,
                                           int columns,
                                           math::ChannelColumnRowTensorReference<ValueType>& result)
        {
            // iterate over entries in the tile
            for (int rowIndex = 0; rowIndex < rows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
                {
                    result((tileRowIndex * tileSize) + rowIndex, (tileColumnIndex * tileSize) + columnIndex, filterIndex) = outputTile(rowIndex, columnIndex);
                }
            }
        }

        static void TransformOutput(const ConstTensorReference& transformedOutput, TensorReference& output)
        {
            const auto numOutputRows = static_cast<int>(output.NumRows());
            const auto numOutputColumns = static_cast<int>(output.NumColumns());
            const auto numFilters = static_cast<int>(output.NumChannels());
            const auto numFullTileRows = numOutputRows / tileSize;
            const auto numFullTileColumns = numOutputColumns / tileSize;
            const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
            const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;

            WindowArray transformedOutputWindow;
            TileArray outputTile;

            // Now un-transform the result, copying tiles into the output
            for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
            {
                for (int tileRowIndex = 0; tileRowIndex < numFullTileRows; ++tileRowIndex)
                {
                    for (int tileColumnIndex = 0; tileColumnIndex < numFullTileColumns; ++tileColumnIndex)
                    {
                        // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
                        // transformedOutput is (wr*wc) x nf x (tr * tc), where tr == # tile rows and tc == # tile columns
                        // transformedOutputWindow is (wr*wc)
                        GetTransformedOutputWindow(transformedOutput, tileRowIndex, tileColumnIndex, filterIndex, numTileRows, numTileColumns, transformedOutputWindow);

                        // Now un-transform window into a tile
                        // outputTile is (tileSize x tileSize)
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformOutputTile(transformedOutputWindow, outputTile);

                        // Copy into output
                        SplatOutputTile(outputTile, tileRowIndex, tileColumnIndex, filterIndex, output);
                    }
                }
            }

            //
            // Now handle partial output tiles
            //

            // First, the bottom row
            const auto ts = tileSize; // STYLE: Necessary to prevent build error on clang
            if (numTileRows > numFullTileRows)
            {
                assert(numTileRows == numFullTileRows + 1);
                int tileRowIndex = numFullTileRows;
                const auto startRowIndex = tileRowIndex * tileSize;
                const auto rows = std::min(ts, static_cast<int>(output.NumRows() - startRowIndex));
                const auto columns = ts;
                for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
                {
                    for (int tileColumnIndex = 0; tileColumnIndex < numFullTileColumns; ++tileColumnIndex)
                    {
                        // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
                        // transformedOutput is (wr*wc) x nf x (tr * tc), where tr == # tile rows and tc == # tile columns
                        // transformedOutputWindow is (wr*wc)
                        GetTransformedOutputWindow(transformedOutput, tileRowIndex, tileColumnIndex, filterIndex, numTileRows, numTileColumns, transformedOutputWindow);

                        // Now un-transform window into a tile
                        // outputTile is (tileSize x tileSize)
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformOutputTile(transformedOutputWindow, outputTile);

                        // Copy into output
                        SplatPartialOutputTile(outputTile, tileRowIndex, tileColumnIndex, filterIndex, rows, columns, output);
                    }
                }
            }

            // Then the righthand column
            if (numTileColumns > numFullTileColumns)
            {
                assert(numTileColumns == numFullTileColumns + 1);
                const int tileColumnIndex = numFullTileColumns;
                const auto startColumnIndex = tileColumnIndex * tileSize;
                const auto rows = ts;
                const auto columns = std::min(ts, static_cast<int>(output.NumColumns() - startColumnIndex));
                for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
                {
                    for (int tileRowIndex = 0; tileRowIndex < numFullTileRows; ++tileRowIndex)
                    {
                        // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
                        // transformedOutput is (wr*wc) x nf x (tr * tc), where tr == # tile rows and tc == # tile columns
                        // transformedOutputWindow is (wr*wc)
                        GetTransformedOutputWindow(transformedOutput, tileRowIndex, tileColumnIndex, filterIndex, numTileRows, numTileColumns, transformedOutputWindow);

                        // Now un-transform window into a tile
                        // outputTile is (tileSize x tileSize)
                        FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformOutputTile(transformedOutputWindow, outputTile);

                        // Copy into output
                        SplatPartialOutputTile(outputTile, tileRowIndex, tileColumnIndex, filterIndex, rows, columns, output);
                    }
                }
            }

            // Finally, the lower-righthand corner
            if (numTileRows > numFullTileRows && numTileColumns > numFullTileColumns)
            {
                assert(numTileRows == numFullTileRows + 1);
                int tileRowIndex = numFullTileRows;
                assert(numTileColumns == numFullTileColumns + 1);
                const int tileColumnIndex = numFullTileColumns;
                const auto startRowIndex = tileRowIndex * tileSize;
                const auto startColumnIndex = tileColumnIndex * tileSize;
                const auto rows = std::min(ts, static_cast<int>(output.NumRows() - startRowIndex));
                const auto columns = std::min(ts, static_cast<int>(output.NumColumns() - startColumnIndex));
                for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
                {
                    // Gather wr x wc slice from tile tr, tc and channel filterIndex of transformedOutput
                    // transformedOutput is (wr*wc) x nf x (tr * tc), where tr == # tile rows and tc == # tile columns
                    // transformedOutputWindow is (wr*wc)
                    GetTransformedOutputWindow(transformedOutput, tileRowIndex, tileColumnIndex, filterIndex, numTileRows, numTileColumns, transformedOutputWindow);

                    // Now un-transform window into a tile
                    // outputTile is (tileSize x tileSize)
                    FixedWinogradTransform2D<ValueType, tileSize, filterSize>::TransformOutputTile(transformedOutputWindow, outputTile);

                    // Copy into output
                    SplatPartialOutputTile(outputTile, tileRowIndex, tileColumnIndex, filterIndex, rows, columns, output);
                }
            }
        }
    };

    // Y = A' * (GgG' .* B'dB) * A
    //
    // g = filter ([g00 g01 g02;  g10 g11 g12;  g20 g21 g22 ])
    // d = signal ([d00 d01 d02 d03;  d10 d11 d12 d13;  d20 d21 d22 d23;  d30 d31 d32 d33 ])
    //

    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, WinogradFilterOrder order)
    {
        const int tileSize = 2;
        return Convolve2DWinograd(signal, filters, numFilters, tileSize, order);
    }

    // filters is a nf x fr x fc x d tensor (represented in 3D as (nf*fr) x fc x d )
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int tileSize, WinogradFilterOrder order)
    {
        using Tensor = math::ChannelColumnRowTensor<ValueType>;

        const auto filterSize = static_cast<int>(filters.NumRows()) / numFilters;
        assert(filterSize == static_cast<int>(filters.NumColumns()) && "Filters must be square");
        const auto outputRows = static_cast<int>(signal.NumRows()) - filterSize + 1;
        const auto outputColumns = static_cast<int>(signal.NumColumns()) - filterSize + 1;
        Tensor result(outputRows, outputColumns, numFilters);

        // In "filtersFirst", transformedFilters is a (nf * wr) x wc x d, where wr == wc == windowSize
        // In "tilesFirst", transformedFilters is a (wr*wc) x nf x d tensor
        Tensor transformedFilters = GetTransformedFilters(filters, numFilters, tileSize, order);

        switch (order)
        {
        case WinogradFilterOrder::filtersFirst:
            Convolve2DWinogradFiltersFirst(signal, transformedFilters, numFilters, tileSize, filterSize, result);
            break;
        case WinogradFilterOrder::tilesFirst:
            Convolve2DWinogradTilesFirst(signal, transformedFilters, numFilters, tileSize, filterSize, result);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        return result;
    }

    // filters is a nf x fr x fc x d tensor (represented in 3D as (nf*fr) x fc x d )
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinogradPretransformed(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int filterSize, WinogradFilterOrder order)
    {
        using Tensor = math::ChannelColumnRowTensor<ValueType>;

        const auto outputRows = static_cast<int>(signal.NumRows()) - filterSize + 1;
        const auto outputColumns = static_cast<int>(signal.NumColumns()) - filterSize + 1;
        Tensor result(outputRows, outputColumns, numFilters);

        switch (order)
        {
        case WinogradFilterOrder::filtersFirst:
            Convolve2DWinogradFiltersFirst(signal, transformedFilters, numFilters, tileSize, filterSize, result);
            break;
        case WinogradFilterOrder::tilesFirst:
            Convolve2DWinogradTilesFirst(signal, transformedFilters, numFilters, tileSize, filterSize, result);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        return result;
    }

    //
    // Straightforward implementation of Winograd algorithm, using separate matrix multiplies to transform
    // each tile
    //
    template <typename ValueType>
    void Convolve2DWinogradFiltersFirstMatrix(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                                              const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                                              int numFilters,
                                              int tileSize,
                                              int filterSize,
                                              math::ChannelColumnRowTensor<ValueType>& result)
    {
        using Matrix = math::RowMatrix<ValueType>;

        const auto windowSize = tileSize + filterSize - 1;
        assert(windowSize == static_cast<int>(transformedFilters.NumRows()) / numFilters);
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
        Matrix outputTile(tileSize, tileSize);
        Matrix X(windowSize, windowSize);
        Matrix signalSlice(signal.NumRows(), signal.NumColumns());
        Matrix transformedFilterSlice(windowSize, windowSize);

        int numTileRows = outputRows / tileSize;
        int numTileColumns = outputColumns / tileSize;
        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            const auto filterOffset = filterIndex * windowSize;
            auto transformedFilter = transformedFilters.GetSubTensor(filterOffset, 0, 0, windowSize, windowSize, numChannels);
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                GetChannelSlice(transformedFilter, channelIndex, transformedFilterSlice);
                GetChannelSlice(signal, channelIndex, signalSlice);
                for (int rowTileIndex = 0; rowTileIndex < numTileRows; ++rowTileIndex)
                {
                    auto rowIndex = rowTileIndex * tileSize;
                    for (int columnTileIndex = 0; columnTileIndex < numTileColumns; ++columnTileIndex)
                    {
                        auto columnIndex = columnTileIndex * tileSize;
                        auto d = signalSlice.GetSubMatrix(rowIndex, columnIndex, windowSize, windowSize);

                        // Compute X = B'dB
                        Multiply(Bt, d, Btd);
                        Multiply(Btd, B, X);

                        // Elementwise multiply transformedFilter * X into X
                        math::ElementwiseMultiplySet(transformedFilterSlice, X, X);

                        // Now Compute result AtXA
                        Multiply(At, X, AtX);
                        Multiply(AtX, A, outputTile);

                        // Similarly to what we did with the input, get the appropriate part of the output,
                        // but append it into the result
                        AccumulateTile(outputTile, rowIndex, columnIndex, filterIndex, result);
                    }
                }
            }
        }
    }

    template <typename ValueType>
    void Convolve2DWinogradFiltersFirst(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                                        const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                                        int numFilters,
                                        int tileSize,
                                        int filterSize,
                                        math::ChannelColumnRowTensor<ValueType>& result)
    {
        using Matrix = math::RowMatrix<ValueType>;

        const auto windowSize = tileSize + filterSize - 1;
        assert(windowSize == static_cast<int>(transformedFilters.NumRows()) / numFilters);
        const auto numChannels = static_cast<int>(signal.NumChannels());
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());
        assert(numFilters == static_cast<int>(result.NumChannels()));

        // Temporary values
        Matrix signalSlice(signal.NumRows(), signal.NumColumns());
        Matrix transformedFilterSlice(windowSize, windowSize); // TODO: use a fixed array
        Matrix X(windowSize, windowSize); // TODO: use a fixed array
        Matrix outputTile(tileSize, tileSize); // TODO: use a fixed array

        int numTileRows = outputRows / tileSize;
        int numTileColumns = outputColumns / tileSize;
        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            const auto filterOffset = filterIndex * windowSize;
            auto transformedFilter = transformedFilters.GetSubTensor(filterOffset, 0, 0, windowSize, windowSize, numChannels);
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                GetChannelSlice(transformedFilter, channelIndex, transformedFilterSlice);
                GetChannelSlice(signal, channelIndex, signalSlice);
                for (int rowTileIndex = 0; rowTileIndex < numTileRows; ++rowTileIndex)
                {
                    auto rowIndex = rowTileIndex * tileSize;
                    for (int columnTileIndex = 0; columnTileIndex < numTileColumns; ++columnTileIndex)
                    {
                        auto columnIndex = columnTileIndex * tileSize;
                        auto d = signalSlice.GetSubMatrix(rowIndex, columnIndex, windowSize, windowSize);

                        if (tileSize == 2 && filterSize == 3)
                        {
                            // Compute X = B'dB
                            FixedWinogradTransform2D<ValueType, 2, 3>::TransformInputWindow(d, X);

                            math::ElementwiseMultiplySet(transformedFilterSlice, X, X);

                            // Now compute result tile Y = At * X * A
                            FixedWinogradTransform2D<ValueType, 2, 3>::TransformOutputTile(X, outputTile);
                        }
                        else if (tileSize == 4 && filterSize == 3)
                        {
                            // Compute B'dB
                            FixedWinogradTransform2D<ValueType, 4, 3>::TransformInputWindow(d, X);

                            math::ElementwiseMultiplySet(transformedFilterSlice, X, X);

                            // Now Compute result tile At * X * A
                            FixedWinogradTransform2D<ValueType, 4, 3>::TransformOutputTile(X, outputTile);
                        }
                        else
                        {
                            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
                        }

                        // Similarly to what we did with the input, get the appropriate part of the output,
                        // but append it into the result
                        AccumulateTile(outputTile, rowIndex, columnIndex, filterIndex, result);
                    }
                }
            }
        }
    }

    template <typename ValueType>
    void TransformInput(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                        int numOutputRows,
                        int numOutputColumns,
                        int numChannels,
                        int tileSize,
                        int filterSize,
                        math::ChannelColumnRowTensor<ValueType>& transformedSignal)
    {
        if (tileSize == 2 && filterSize == 3)
        {
            FixedWinograd2D<ValueType, 2, 3>::TransformInput(signal, numOutputRows, numOutputColumns, numChannels, transformedSignal);
        }
        else if (tileSize == 4 && filterSize == 3)
        {
            FixedWinograd2D<ValueType, 4, 3>::TransformInput(signal, numOutputRows, numOutputColumns, numChannels, transformedSignal);
        }
        else
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
    }

    template <typename ValueType>
    void ComputeTransformedOutput(const math::ConstChannelColumnRowTensorReference<ValueType>& transformedSignal,
                                  const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                                  int numOutputRows,
                                  int numOutputColumns,
                                  int numChannels,
                                  int numFilters,
                                  int tileSize,
                                  int filterSize,
                                  math::ChannelColumnRowTensor<ValueType>& transformedOutput)
    {
        const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
        const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;
        DEBUG_USED(numChannels, numFilters, numTileRows, numTileColumns);

        const auto windowSize = filterSize + tileSize - 1;

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
    }

    template <typename ValueType>
    void TransformOutput(const math::ConstChannelColumnRowTensorReference<ValueType>& transformedOutput,
                         int tileSize,
                         int filterSize,
                         math::ChannelColumnRowTensor<ValueType>& result)
    {

        if (tileSize == 2 && filterSize == 3)
        {
            FixedWinograd2D<ValueType, 2, 3>::TransformOutput(transformedOutput, result);
        }
        else if (tileSize == 4 && filterSize == 3)
        {
            FixedWinograd2D<ValueType, 4, 3>::TransformOutput(transformedOutput, result);
        }
        else
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
    }

    //
    // More efficient version (especially when the number of channels and filters is large) that
    // pretransforms all of the input tiles, uses a series of GEMM calls to accumulate the channels
    // of the filtered output, and then transforms the output.
    //
    template <typename ValueType>
    void Convolve2DWinogradTilesFirst(const math::ConstChannelColumnRowTensorReference<ValueType>& signal,
                                      const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters,
                                      int numFilters,
                                      int tileSize,
                                      int filterSize,
                                      math::ChannelColumnRowTensor<ValueType>& result)
    {
        using Tensor = math::ChannelColumnRowTensor<ValueType>;

        // transformedFilters is a (wr*wc) x nf x d tensor
        // result is a rows x columns x nf tensor
        const auto numOutputRows = static_cast<int>(result.NumRows());
        const auto numOutputColumns = static_cast<int>(result.NumColumns());
        const auto numChannels = static_cast<int>(signal.NumChannels());
        const auto windowSize = filterSize + tileSize - 1;
        const auto numTileRows = ((numOutputRows - 1) / tileSize) + 1;
        const auto numTileColumns = ((numOutputColumns - 1) / tileSize) + 1;
        assert(numFilters == static_cast<int>(result.NumChannels()));
        assert(numFilters == static_cast<int>(transformedFilters.NumColumns()));
        assert(numChannels == static_cast<int>(transformedFilters.NumChannels()));

        // transformedSignal is a (wr*wc) x d x (tr * tc) tensor containing the entire transformed input signal
        Tensor transformedSignal(windowSize * windowSize, numChannels, numTileRows * numTileColumns);
        TransformInput(signal, numOutputRows, numOutputColumns, numChannels, tileSize, filterSize, transformedSignal);

        // transformedOutput is (wr*wc) x nf x (tr * tc)
        Tensor transformedOutput(windowSize * windowSize, numFilters, numTileRows * numTileColumns);
        ComputeTransformedOutput(transformedSignal, transformedFilters, numOutputRows, numOutputColumns, numChannels, numFilters, tileSize, filterSize, transformedOutput);

        // Un-transform convolved output and write into output image
        TransformOutput(transformedOutput, tileSize, filterSize, result);
    }
}
}
