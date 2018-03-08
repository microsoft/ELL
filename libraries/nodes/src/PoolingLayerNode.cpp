////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PoolingLayerNode.h"
#include "ConstantNode.h"

// predictors
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"

namespace ell
{
namespace nodes
{
    namespace
    {
        struct Interval
        {
            int begin;
            int end;
        };

        struct RegionBounds
        {
            Interval inputBounds;
            Interval windowBounds;
        };

        RegionBounds GetRegionBounds(int regionId, int inputSize, int windowSize, int stride, bool usesPadding)
        {
            RegionBounds result;
            const int negWindowExtent = ((-windowSize + 1) / 2);
            const int posWindowExtent = windowSize / 2;

            // Default values for the 0 (middle) region. This is the region where the entire window covers valid input.
            //
            result.inputBounds.begin = -negWindowExtent;
            result.inputBounds.end = inputSize - posWindowExtent;
            result.windowBounds.begin = negWindowExtent;
            result.windowBounds.end = posWindowExtent + 1; // +1 because we use half-open intervals: [begin, end)

            if (usesPadding)
            {
                if (regionId < 0)
                {
                    result.inputBounds.begin += regionId;
                    result.inputBounds.end = result.inputBounds.begin + 1;
                    result.windowBounds.begin -= regionId;
                }
                else if (regionId > 0)
                {
                    result.inputBounds.begin = inputSize - posWindowExtent + regionId - 1;
                    result.inputBounds.end = result.inputBounds.begin + 1;
                    result.windowBounds.end -= regionId;
                }
            }
            else
            {
                if (regionId == 0)
                {
                    // nothing
                }
                else
                {
                    result.inputBounds.end = result.inputBounds.begin - stride - 1;
                }
            }
            return result;
        }
    } // end anonymous namespace

    //
    // Note: the pooling function constructors take a `staticCount` parameter. This parameter
    // tells how many valid data values the function will be evaluated over in order to compute
    // the output of one pooling window. It's currently only used to determine the denominator for the
    // mean pooling. The pooling functions are able to work in a mode where they know ahead of time
    // how many values they'll be evaluated over, and also in a mode where they have to count it
    // dynamically. If nonnegative, the `staticCount` parameter tells how many times the function
    // will be called (via `Accumulate`) for one pooling window. If `staticCount` < 0, then the function
    // operates in the dynamic mode where it keeps a count of the number of times its been called.

    //
    // MaxPoolingFunction
    //
    template <typename ValueType>
    class MaxPoolingFunction
    {
    public:
        MaxPoolingFunction(emitters::IRFunctionEmitter& function, int staticCount, ValueType paddingValue = 0)
        {
            auto valueType = emitters::GetVariableType<ValueType>();
            _accumValueVar = function.Variable(valueType, "poolingAccumValue");
            _paddingValue = function.LocalScalar<ValueType>(paddingValue);
            _initialValue = function.LocalScalar<ValueType>(std::numeric_limits<ValueType>::lowest());
            Reset(function, staticCount);
        }

        void Reset(emitters::IRFunctionEmitter& function, int count)
        {
            function.Store(_accumValueVar, _initialValue);
        }

        void Accumulate(emitters::IRFunctionEmitter& function, llvm::Value* value)
        {
            auto ifEmitter = function.If();
            ifEmitter.If(emitters::TypedComparison::greaterThanFloat, value, function.Load(_accumValueVar));
            {
                function.Store(_accumValueVar, value);
            }
            ifEmitter.End();
        }

        llvm::Value* GetValueAtPadding(emitters::IRFunctionEmitter& function)
        {
            return _paddingValue;
        }

        llvm::Value* GetValue(emitters::IRFunctionEmitter& function)
        {
            return function.Load(_accumValueVar);
        }

    private:
        llvm::Value* _accumValueVar;
        llvm::Value* _initialValue;
        llvm::Value* _paddingValue;
    };

    //
    // MeanPoolingFunction
    //
    template <typename ValueType>
    class MeanPoolingFunction
    {
    public:
        MeanPoolingFunction(emitters::IRFunctionEmitter& function, int staticCount, ValueType paddingValue = 0)
            : _count(staticCount)
        {
            auto valueType = emitters::GetVariableType<ValueType>();
            _paddingValue = function.LocalScalar(paddingValue);
            _accumValueVar = function.Variable(valueType, "poolingAccumValue");
            // if we don't know how many times we'll be called, create a variable (_countVar) to keep track
            _countVar = staticCount < 0 ? function.Variable(emitters::VariableType::Int32, "poolingAccumCount") : nullptr;
            Reset(function, staticCount);
        }

        void Reset(emitters::IRFunctionEmitter& function, int staticCount)
        {
            _count = staticCount;
            function.StoreZero(_accumValueVar);
            if (_countVar != nullptr)
            {
                function.StoreZero(_countVar);
            }
        }

        void Accumulate(emitters::IRFunctionEmitter& function, llvm::Value* value)
        {
            const auto plusFloat = emitters::TypedOperator::addFloat;
            function.OperationAndUpdate(_accumValueVar, plusFloat, value);
            if (_countVar != nullptr)
            {
                const auto plus = emitters::TypedOperator::add;
                function.OperationAndUpdate(_countVar, plus, function.Literal<int>(1));
            }
        }

        llvm::Value* GetValueAtPadding(emitters::IRFunctionEmitter& function)
        {
            assert(_countVar != nullptr && "GetValueAtPadding shouldn't be called for this pooling function");
            return _paddingValue;
        }

        llvm::Value* GetValue(emitters::IRFunctionEmitter& function)
        {
            if (_countVar == nullptr)
            {
                return function.Operator(emitters::TypedOperator::divideFloat, function.Load(_accumValueVar), function.LocalScalar<ValueType>(_count));
            }
            else
            {
                return function.Operator(emitters::TypedOperator::divideFloat, function.Load(_accumValueVar), function.CastValue<int, ValueType>(function.Load(_countVar)));
            }
        }

    private:
        llvm::Value* _paddingValue;
        llvm::Value* _accumValueVar;
        llvm::Value* _countVar;
        int _count;
    };

    // Silly type_traits-like thing to transform predictors::neural::MaxPoolingFunction -> MaxPoolingFunction
    template <template <typename> class PoolingFunctionType, typename ValueType>
    struct PoolingFunctionT
    {
    };

    template <typename ValueType>
    struct PoolingFunctionT<predictors::neural::MaxPoolingFunction, ValueType>
    {
        using type = MaxPoolingFunction<ValueType>;
    };

    template <typename ValueType>
    struct PoolingFunctionT<predictors::neural::MeanPoolingFunction, ValueType>
    {
        using type = MeanPoolingFunction<ValueType>;
    };

    template <template <typename> class PoolingFunctionType, typename ValueType>
    bool IsMaxPoolingFunction(const PoolingFunctionType<ValueType>& function)
    {
        return std::is_same<PoolingFunctionType<ValueType>, MaxPoolingFunction<ValueType>>();
    }

    //
    // PoolingLayerNode
    //

    template <typename ValueType, template <typename> class PoolingFunctionType>
    PoolingLayerNode<ValueType, PoolingFunctionType>::PoolingLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>& layer)
        : NeuralNetworkLayerNode<PoolingLayerNode<ValueType, PoolingFunctionType>, predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>, ValueType>(input, layer)
    {
    }

    // inputRow, inputColumn, and inputChannel
    template <typename ValueType, template <typename> class PoolingFunctionType>
    template <typename PoolingFunctionT>
    llvm::Value* PoolingLayerNode<ValueType, PoolingFunctionType>::GetPoolingWindowValue(emitters::IRFunctionEmitter& function,
                                                                                         int windowRowBegin,
                                                                                         int windowRowEnd,
                                                                                         int windowColumnBegin,
                                                                                         int windowColumnEnd,
                                                                                         llvm::Value* inputRow,
                                                                                         llvm::Value* inputColumn,
                                                                                         llvm::Value* inputChannel,
                                                                                         llvm::Value* inputBuffer,
                                                                                         const model::Shape& inputIncrement,
                                                                                         PoolingFunctionT& poolingFunction)
    {
        const auto plus = emitters::TypedOperator::add;
        const auto times = emitters::TypedOperator::multiply;

        // Number of cells in this pooling window
        int numCells = (windowRowEnd - windowRowBegin) * (windowColumnEnd - windowColumnBegin);
        poolingFunction.Reset(function, numCells);

        // Window size and stride
        auto poolingParameters = this->GetLayer().GetPoolingParameters();
        int windowSize = poolingParameters.poolingSize;

        bool hasFullWindow = (numCells == windowSize * windowSize);
        if (!hasFullWindow && IsMaxPoolingFunction(poolingFunction))
        {
            poolingFunction.Accumulate(function, poolingFunction.GetValueAtPadding(function));
        }

        // Double-loop to iterate over each entry in the pooling window. The middle of the window is (0,0)
        for (int poolingRow = windowRowBegin; poolingRow < windowRowEnd; ++poolingRow) // in [-w/2, w/2]
        {
            for (int poolingColumn = windowColumnBegin; poolingColumn < windowColumnEnd; ++poolingColumn)
            {
                // poolingInputRow and poolingInputColumn are the coordinates of the input image value to
                // accumulate for this entry in the pooling window
                auto poolingInputRow = function.Operator(plus, inputRow, function.LocalScalar<int>(poolingRow));
                auto poolingInputColumn = function.Operator(plus, inputColumn, function.LocalScalar<int>(poolingColumn));

                // auto totalOffset = (offsetX * inputIncrement[0]) + (offsetY * inputIncrement[1]);
                auto inputIndex = function.Operator(plus, function.Operator(plus, function.Operator(times, poolingInputRow, function.LocalScalar<int>(inputIncrement[0])), function.Operator(times, poolingInputColumn, function.LocalScalar<int>(inputIncrement[1]))), inputChannel);
                auto value = function.ValueAt(inputBuffer, inputIndex);
                poolingFunction.Accumulate(function, value);
            }
        }

        auto value = poolingFunction.GetValue(function);
        return value;
    }

    /*

    # Splitting windowed operations into regions

    When scanning a window of size w over a signal (assume size >=w), there are
    2w-1 possible 'regions', or amounts of overlap between the window and the
    signal:
      * An infinite region w or more entries to the left of the signal, where there is no overlap
      * w-1 regions of size 1, where there is an overlap amount between 1 and (w-1)
      * 1 region of size (signal width - w + 1) where the window and signal completely overlap (an overlap of size w)
      * w-1 more regions of size 1, where there is between (w-1) and 1 elements of overlap, on the other side of the window
      * Another infinite region to the right of the signal, where there is no overlap

    In practice, we can ignore many of these regions. If the 'pad' setting is 'false', we only need
    to deal with the middle region of full overlap.

    When 'pad' is 'true', we can ignore the infinite non-overlap regions and ~half of the size-1
    regions. The infinite regions are obvious. We can also ignore the size-1 regions where the center of
    the window is outside the input signal. This gives us w regions:

      * floor(w/2) regions where the left part of the window doesn't overlap with the signal
      * 1 region with full overlap
      * ceil((w-1)/2) regions where the right part of the window doesn't overlap


    ## To compute:
    ```
    For each region r:
      Intersect the region's extent with the signal's extent, to get a range of included input locations.
      Using stride/offset/whatever, map input extent into extent of output locations, being careful about right-hand bound.
      if output extent empty, continue
      for each output location in output extent:
        reverse-map output location to get input location
        look up value at input location
        evaluate function
        accumulate
        [Note that we can easily derive the number of cells visited in this inner loop from the region,
          so there is no need to count cells for operations like 'mean']
    ```

    ## Dealing with 'pad' parameter:
    If 'pad' is 'true', output location 0 corresponds to a window centered on input location 0.

    if 'pad' is 'false', output location 0 corresponds to a window with its leftmost cell at
    input location 0. Equivalently, the window is centered at input location w/2.

    Let's consider 'pad' = 'true' to be the canonical setting. Then if 'pad' is 'false', we just need
    to offset the coordinate-mapping by w/2 (that is, output(0) -> window centered at input(w/2)). We
    need to take this mapping into account when computing the output extents for the regions.

    ## Dealing with explicit input padding:
    Often, the input data is padded with zeros (or another appropriate value) in order for the inner loop
    to always be able to loop over all window pixels. To deal with this situation, we can just modify the
    input data bounds to include the appropriate amount of padded data. Then the algorithm will proceed as
    in the case where 'pad' is 'false'. Note that we can easily accomodate hybrid situations, where the input
    data has been padded with zero values, but not enough data padding has been supplied. Then we just adjust
    the input size and proceed, and the algorithm will just use fewer regions than it would otherwise (and more
    data will fall into the middle, 'full-window' region).

    ## Dealing with stride != 1:
    Non-1 stride changes the output->input mapping by simply scaling the output value by the stride. However,
    it's important to be careful when doing the region extent mapping to make sure we don't leave off
    the rightmost piece of the output region when truncating. (Does using half-open intervals save us here? I think so.)

    ## Customizing to deal with differences in other frameworks:
    If other frameworks have different ways of centering windows, we can deal with them in a couple of ways:
      * Explicitly pad the data with zero values in such a way that we get the same result as the framework we're trying
        to match
      * Add a 'offset' parameter that modifies the input<-->output mapping. For instance, if a framework decided that the
        'middle' pixel of an even-sized window was the one to the right of center, we would have an offset of 1 (or -1)

    ## On the numbering scheme for regions:
    Regions are identified by a region number or ID, which has valid values in the interval [-(w+1), (w+1)]. If a region
    number is negative, it indicates how many elements from the negative (left-hand) side of the window are outside of the
    input signal's area. If the region number is positive, it indicates how many elements on the right-hand side of the
    window are outside of the image area. A region ID of zero corresponds to the middle area of the input signal where the
    window is completely contained inside the image area.

    Example:
    Consider a pooling operation that pools values from a 3x3 window, operating on an 8x8 image:


                                         +---+---+---+---+---+---+---+---+
                                         |   |   |   |   |   |   |   |   |
                                         +---+---+---+---+---+---+---+---+
                                         |   |   |   |   |   |   |   |   |
            +---+---+---+                +---+---+---+---+---+---+---+---+
            |   |   |   |                |   |   |   |   |   |   |   |   |
            +---+---+---+                +---+---+---+---+---+---+---+---+
    Window: |   | x |   |        Signal: |   |   |   |   |   |   |   |   |
            +---+---+---+                +---+---+---+---+---+---+---+---+
            |   |   |   |                |   |   |   |   |   |   |   |   |
            +---+---+---+                +---+---+---+---+---+---+---+---+
                                         |   |   |   |   |   |   |   |   |
                                         +---+---+---+---+---+---+---+---+
                                         |   |   |   |   |   |   |   |   |
                                         +---+---+---+---+---+---+---+---+
                                         |   |   |   |   |   |   |   |   |
                                         +---+---+---+---+---+---+---+---+


             +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
             |-1,-1|                     -1,0                      | -1,1|
             +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
    Regions: | 0,-1|                     0,0                       | 0,1 |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +     +     +     +     +     +     +     +     +     +     +
             |     |                                               |     |
             +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
             | 1,-1|                      1,0                      | 1,1 |
             +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */

    template <typename ValueType, template <typename> class PoolingFunctionType>
    void PoolingLayerNode<ValueType, PoolingFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // convenience operator names
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Input / output memory layouts
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.GetActiveSize();
        const auto& inputOffset = inputLayout.GetOffset();

        const auto& outputLayout = this->GetOutputMemoryLayout();
        const auto& outputSize = outputLayout.GetActiveSize();
        const auto& outputOffset = outputLayout.GetOffset();

        // Calculate cumulative increment for each dimension
        model::Shape inputIncrement = inputLayout.GetCumulativeIncrement();
        model::Shape outputIncrement = outputLayout.GetCumulativeIncrement();

        // Calculate input dimension parameters
        int inputRows = inputSize[0];
        int inputColumns = inputSize[1];
        int inputDepth = inputSize[2];

        int outputRows = outputSize[0];
        int outputColumns = outputSize[1];
        int outputDepth = outputSize[2];
        UNUSED(outputRows, outputColumns);

        if (inputDepth != outputDepth)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output of pooling layer must have same depth");
        }

        // Window size and stride
        auto poolingParameters = this->GetLayer().GetPoolingParameters();
        int stride = poolingParameters.stride;
        int windowSize = poolingParameters.poolingSize;

        // Create the pooling function
        using FType = typename PoolingFunctionT<PoolingFunctionType, ValueType>::type;
        auto paddingValue = ell::predictors::neural::GetPaddingValue<ValueType>(this->GetLayer().GetLayerParameters().inputPaddingParameters.paddingScheme);
        FType poolingFunction{ function, windowSize * windowSize, paddingValue }; // Create the pooling function with a 'full' pooling window.

        // These "window extent" variables indicate the amount that the pooling window extends to the left and right (or top/bottom) of the center pixel.
        //   posWindowExtent is always floor(windowSize/2)
        //   If the pooling window size is odd, negWindowExtent == -posWindowExtent,
        //   if the pooling window size is even, negWindowExtent == -(posWindowExtent - 1)
        const int negWindowExtent = ((-windowSize + 1) / 2);
        const int posWindowExtent = windowSize / 2;

        const bool usesPadding = GetLayer().UsesPadding();

        // Pointers to beginning of 'active' area of input and output
        const auto inputBufferOffset = (inputIncrement[0] * inputOffset[0]) + (inputIncrement[1] * inputOffset[1]) + (inputIncrement[2] * inputOffset[2]);
        const auto outputBufferOffset = (outputIncrement[0] * outputOffset[0]) + (outputIncrement[1] * outputOffset[1]) + (outputIncrement[2] * outputOffset[2]);
        auto inputBuffer = function.PointerOffset(pInput, inputBufferOffset);
        auto outputBuffer = function.PointerOffset(pOutput, outputBufferOffset);

        // Divide the output into regions that have different support over the pooling window. There are `windowSize` regions in each dimension.
        for (int rowsRegion = negWindowExtent; rowsRegion <= posWindowExtent; ++rowsRegion)
        {
            auto rowRegionBounds = GetRegionBounds(rowsRegion, inputRows, windowSize, stride, usesPadding);
            for (int colsRegion = negWindowExtent; colsRegion <= posWindowExtent; ++colsRegion)
            {
                auto columnRegionBounds = GetRegionBounds(colsRegion, inputColumns, windowSize, stride, usesPadding);
                const int padOffset = usesPadding ? 0 : -negWindowExtent;
                // Now the current input region is bounded by [rowRegionBounds.inputBounds.begin, rowRegionBounds.inputBounds.end) x [columnRegionBounds.inputBounds.begin, inputColEnd]
                int minInputRow = std::max(rowRegionBounds.inputBounds.begin, 0);
                int maxInputRow = std::min(rowRegionBounds.inputBounds.end, inputRows);
                int minInputCol = std::max(columnRegionBounds.inputBounds.begin, 0);
                int maxInputCol = std::min(columnRegionBounds.inputBounds.end, inputColumns);
                auto minOutputRow = (minInputRow - padOffset + stride - 1) / stride;
                auto maxOutputRow = (maxInputRow - padOffset - 1) / stride + 1;
                auto minOutputCol = (minInputCol - padOffset + stride - 1) / stride;
                auto maxOutputCol = (maxInputCol - padOffset - 1) / stride + 1;

                if (maxOutputRow > minOutputRow && maxOutputCol > minOutputCol)
                {
                    // BUG: explicit by-ref captures of `usesPadding` and `negWindowExtent` are here to work around a GCC bug
                    function.For(minOutputRow, maxOutputRow, 1, [=, &usesPadding, &negWindowExtent, &poolingFunction](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
                        auto outputRow = function.LocalScalar(loopIndex1);
                        auto inputRow = outputRow * function.LocalScalar<int>(stride);
                        if (!usesPadding)
                        {
                            inputRow = inputRow + function.LocalScalar<int>(-negWindowExtent);
                        }

                        function.For(minOutputCol, maxOutputCol, 1, [=, &poolingFunction, &inputRow](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                            auto outputColumn = function.LocalScalar(loopIndex2);
                            auto inputColumn = outputColumn * function.LocalScalar<int>(stride);
                            if (!usesPadding)
                            {
                                inputColumn = inputColumn + function.LocalScalar<int>(-negWindowExtent);
                            }

                            function.For(outputDepth, [=, &poolingFunction, &inputRow, &inputColumn](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex3) {
                                auto channel = function.LocalScalar(loopIndex3);
                                // Get the pooled value
                                auto pooledValue = GetPoolingWindowValue(function, rowRegionBounds.windowBounds.begin, rowRegionBounds.windowBounds.end, columnRegionBounds.windowBounds.begin, columnRegionBounds.windowBounds.end, inputRow, inputColumn, channel, inputBuffer, inputIncrement, poolingFunction);
                                // and store it in the output
                                auto outputIndex = (outputRow * function.LocalScalar<int>(outputIncrement[0])) +
                                                   (outputColumn * function.LocalScalar<int>(outputIncrement[1])) +
                                                   channel;
                                function.SetValueAt(outputBuffer, outputIndex, pooledValue);
                            });
                        });
                    });
                }
            }
        }
    } // end function

    // Explicit specialization
    template class MeanPoolingFunction<float>;
    template class MeanPoolingFunction<double>;
    template class MaxPoolingFunction<float>;
    template class MaxPoolingFunction<double>;

    template class PoolingLayerNode<float, ell::predictors::neural::MeanPoolingFunction>;
    template class PoolingLayerNode<double, ell::predictors::neural::MeanPoolingFunction>;
    template class PoolingLayerNode<float, ell::predictors::neural::MaxPoolingFunction>;
    template class PoolingLayerNode<double, ell::predictors::neural::MaxPoolingFunction>;
} // nodes
} // ell
