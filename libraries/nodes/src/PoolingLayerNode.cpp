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
    template <typename ValueType>
    class MaxPoolingFunction
    {
    public:
        MaxPoolingFunction(emitters::IRFunctionEmitter& function)
        {
            auto valueType = emitters::GetVariableType<ValueType>();
            _accumValueVar = function.Variable(valueType, "poolingAccumValue");
            Reset(function);
        }

        void Reset(emitters::IRFunctionEmitter& function)
        {
            function.Store(_accumValueVar, function.Literal(-(std::numeric_limits<ValueType>::max())));
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
            return function.Literal<ValueType>(-(std::numeric_limits<ValueType>::max()));
        }

        llvm::Value* GetValue(emitters::IRFunctionEmitter& function)
        {
            return function.Load(_accumValueVar);
        }

    private:
        llvm::Value* _accumValueVar;
    };

    template <typename ValueType>
    class MeanPoolingFunction
    {
    public:
        MeanPoolingFunction(emitters::IRFunctionEmitter& function)
        {
            auto valueType = emitters::GetVariableType<ValueType>();
            _accumValueVar = function.Variable(valueType, "poolingAccumValue");
            _countVar = function.Variable(emitters::VariableType::Int32, "poolingAccumCount");
            Reset(function);
        }

        void Reset(emitters::IRFunctionEmitter& function)
        {
            function.Store(_accumValueVar, function.Literal<ValueType>(0));
            function.Store(_countVar, function.Literal(0));
        }

        void Accumulate(emitters::IRFunctionEmitter& function, llvm::Value* value)
        {
            const auto plusFloat = emitters::TypedOperator::addFloat;
            const auto plus = emitters::TypedOperator::add;
            function.OperationAndUpdate(_accumValueVar, plusFloat, value);
            function.OperationAndUpdate(_countVar, plus, function.Literal(1));
        }

        llvm::Value* GetValueAtPadding(emitters::IRFunctionEmitter& function)
        {
            return function.Literal<ValueType>(0);
        }

        llvm::Value* GetValue(emitters::IRFunctionEmitter& function)
        {
            return function.Operator(emitters::TypedOperator::divideFloat, function.Load(_accumValueVar), function.CastValue<int, ValueType>(function.Load(_countVar)));
        }

    private:
        llvm::Value* _accumValueVar;
        llvm::Value* _countVar;
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

    //
    // PoolingLayerNode
    //

    template <typename ValueType, template <typename> class PoolingFunctionType>
    PoolingLayerNode<ValueType, PoolingFunctionType>::PoolingLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>& layer)
        : NeuralNetworkLayerNode<PoolingLayerNode<ValueType, PoolingFunctionType>, predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType, template <typename> class PoolingFunctionType>
    void PoolingLayerNode<ValueType, PoolingFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // convenience operator names
        const auto plus = emitters::TypedOperator::add;
        const auto times = emitters::TypedOperator::multiply;
        const auto greaterThanOrEqual = emitters::TypedComparison::greaterThanOrEquals;
        const auto lessThan = emitters::TypedComparison::lessThan;

        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Input / output memory layouts
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.size;
        const auto& inputStride = inputLayout.stride;
        const auto& inputOffset = inputLayout.offset;

        const auto& outputLayout = this->GetOutputMemoryLayout();
        const auto& outputSize = outputLayout.size;
        const auto& outputStride = outputLayout.stride;
        const auto& outputOffset = outputLayout.offset;

        // Calculate cumulative increment for each dimension
        Shape inputIncrement = inputLayout.GetCumulativeIncrement();
        Shape outputIncrement = outputLayout.GetCumulativeIncrement();

        const auto& layerParameters = this->GetLayer().GetLayerParameters();

        // Calculate input dimension parameters
        size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;

        int inputDepth = inputSize[2];
        int inputColumns = inputSize[1];
        int inputRows = inputSize[0];

        int outputDepth = outputSize[2];
        int outputColumns = outputSize[1];
        int outputRows = outputSize[0];

        if (inputDepth != outputDepth)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output of pooling layer must have same depth");
        }

        auto poolingParameters = this->GetLayer().GetPoolingParameters();
        int stride = poolingParameters.stride;
        int poolingSize = poolingParameters.poolingSize;

        int paddingColumnOffset = -inputPaddingSize;
        int paddingRowOffset = -inputPaddingSize;

        // If the incoming memory is larger enough that our pooling windows never exceed its bounds, we can skip the bounds check (and assume the extra memory contains the correct padding value)
        bool canSkipBoundsCheck = (inputPaddingSize <= inputOffset[0]) &&
                                  ((outputColumns - 1) * stride + 1 + (poolingSize / 2) <= (inputColumns + 2 * inputPaddingSize)) &&
                                  ((outputRows - 1) * stride + 1 + (poolingSize / 2) <= (inputRows + 2 * inputPaddingSize));

        // TODO: add prologue / epilogue for padded / out-of-bounds values

        // Create the pooling function
        using FType = typename PoolingFunctionT<PoolingFunctionType, ValueType>::type;
        FType poolingFunction{ function };

        // TODO: implement these nested loops via recursion
        const int rowDimension = 0;
        const int columnDimension = 1;
        const int channelDimension = 2;

        auto rowLoop = function.ForLoop();
        rowLoop.Begin(outputRows); // for each row
        {
            auto outputRowIndex = rowLoop.LoadIterationVariable();
            auto inputRowIndex = function.Operator(times, outputRowIndex, function.Literal(stride));

            llvm::Value* rowInputInternalOffset = function.Operator(plus, inputRowIndex, function.Literal<int>(inputOffset[rowDimension]));
            llvm::Value* rowOutputInternalOffset = function.Operator(plus, outputRowIndex, function.Literal<int>(outputOffset[rowDimension]));

            llvm::Value* rowInputOffset = function.Operator(times, rowInputInternalOffset, function.Literal<int>(inputIncrement[rowDimension]));
            llvm::Value* rowOutputOffset = function.Operator(times, rowOutputInternalOffset, function.Literal<int>(outputIncrement[rowDimension]));

            auto columnLoop = function.ForLoop();
            columnLoop.Begin(outputColumns); // for each column
            {
                auto outputColumnIndex = columnLoop.LoadIterationVariable();
                auto inputColumnIndex = function.Operator(times, outputColumnIndex, function.Literal(stride));

                llvm::Value* columnInputInternalOffset = function.Operator(plus, inputColumnIndex, function.Literal<int>(inputOffset[columnDimension]));
                auto scaledColumnInputOffset = function.Operator(times, columnInputInternalOffset, function.Literal<int>(inputIncrement[columnDimension]));
                auto columnInputOffset = function.Operator(plus, rowInputOffset, scaledColumnInputOffset);

                llvm::Value* columnOutputInternalOffset = function.Operator(plus, outputColumnIndex, function.Literal<int>(outputOffset[columnDimension]));
                auto scaledColumnOutputOffset = function.Operator(times, columnOutputInternalOffset, function.Literal<int>(outputIncrement[columnDimension]));
                auto columnOutputOffset = function.Operator(plus, rowOutputOffset, scaledColumnOutputOffset);

                auto channelLoop = function.ForLoop();
                channelLoop.Begin(inputDepth); // for each channel
                {
                    auto channelIndex = channelLoop.LoadIterationVariable();

                    // Note that channel stride == 1, so we don't really need to scale it. The optimizer should get rid of the unnecessary multiply by 1
                    llvm::Value* channelInputInternalOffset = function.Operator(plus, channelIndex, function.Literal<int>(inputOffset[channelDimension]));
                    auto scaledChannelInputOffset = function.Operator(times, channelInputInternalOffset, function.Literal<int>(inputIncrement[channelDimension]));
                    auto channelInputOffset = function.Operator(plus, columnInputOffset, scaledChannelInputOffset);

                    llvm::Value* channelOutputInternalOffset = function.Operator(plus, channelIndex, function.Literal<int>(outputOffset[channelDimension]));
                    auto scaledChannelOutputOffset = function.Operator(times, channelOutputInternalOffset, function.Literal<int>(outputIncrement[channelDimension]));
                    auto channelOutputOffset = function.Operator(plus, columnOutputOffset, scaledChannelOutputOffset);

                    // inputLocationOffset is the offset to the beginning corner of the input window
                    // outputLocationOffset is the offset to the output entry
                    llvm::Value* inputLocationOffset = channelInputOffset;
                    llvm::Value* outputLocationOffset = channelOutputOffset;

                    // Now loop over the input window
                    //
                    poolingFunction.Reset(function);
                    for (int poolRowIndex = 0; poolRowIndex < poolingSize; ++poolRowIndex)
                    {
                        for (int poolColumnIndex = 0; poolColumnIndex < poolingSize; ++poolColumnIndex)
                        {
                            int offsetX = poolColumnIndex + paddingColumnOffset; // ix - padding
                            int offsetY = poolRowIndex + paddingRowOffset; // iy - padding
                            auto totalOffset = (offsetX * inputIncrement[0]) + (offsetY * inputIncrement[1]);

                            if (canSkipBoundsCheck)
                            {
                                auto valueIndex = function.Operator(plus, inputLocationOffset, function.Literal<int>(totalOffset));
                                auto value = function.ValueAt(pInput, valueIndex);
                                poolingFunction.Accumulate(function, value);
                            }
                            else
                            {
                                //
                                // This is a bit of a mess, but it works
                                //

                                auto xCoordinate = function.Operator(plus, function.Literal<int>(offsetX), inputColumnIndex);
                                auto yCoordinate = function.Operator(plus, function.Literal<int>(offsetY), inputRowIndex);

                                auto xTooSmall = function.Comparison(lessThan, xCoordinate, function.Literal<int>(0));
                                auto xTooBig = function.Comparison(greaterThanOrEqual, xCoordinate, function.Literal(inputColumns));
                                auto yTooSmall = function.Comparison(lessThan, yCoordinate, function.Literal<int>(0));
                                auto yTooBig = function.Comparison(greaterThanOrEqual, yCoordinate, function.Literal(inputRows));
                                auto xBad = function.Operator(emitters::TypedOperator::logicalOr, xTooSmall, xTooBig);
                                auto yBad = function.Operator(emitters::TypedOperator::logicalOr, yTooSmall, yTooBig);
                                auto outOfBounds = function.Operator(emitters::TypedOperator::logicalOr, xBad, yBad);

                                auto ifEmitter = function.If();
                                ifEmitter.If(outOfBounds, true);
                                {
                                    auto paddingValue = poolingFunction.GetValueAtPadding(function);
                                    poolingFunction.Accumulate(function, paddingValue);
                                }
                                ifEmitter.Else();
                                {
                                    auto valueIndex = function.Operator(plus, inputLocationOffset, function.Literal<int>(totalOffset));
                                    auto value = function.ValueAt(pInput, valueIndex);
                                    poolingFunction.Accumulate(function, value);
                                }
                                ifEmitter.End();
                            }
                        }
                    }

                    auto value = poolingFunction.GetValue(function);
                    function.SetValueAt(pOutput, outputLocationOffset, value);
                }
                channelLoop.End();
            }
            columnLoop.End();
        }
        rowLoop.End();

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
