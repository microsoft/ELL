////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SoftmaxLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"

namespace ell
{
namespace nodes
{
    namespace
    {
        //
        // Function objects for separate phases
        //

        template <typename ValueType>
        class FindMaxFunction
        {
        public:
            FindMaxFunction(emitters::IRFunctionEmitter& function)
            {
                auto valueType = emitters::GetVariableType<ValueType>();
                _accumValueVar = function.Variable(valueType, "maxAccumValue");
                Reset(function);
            }

            void Reset(emitters::IRFunctionEmitter& function)
            {
                function.Store(_accumValueVar, function.Literal(-(std::numeric_limits<ValueType>::max())));
            }

            llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x)
            {
                auto ifEmitter = function.If();
                ifEmitter.If(emitters::TypedComparison::greaterThanFloat, x, function.Load(_accumValueVar));
                {
                    function.Store(_accumValueVar, x);
                }
                ifEmitter.End();
                return nullptr;
            }

            llvm::Value* GetMaxValue(emitters::IRFunctionEmitter& function) const
            {
                return function.Load(_accumValueVar);
            }

        private:
            llvm::Value* _accumValueVar;
        };

        template <typename ValueType>
        class ComputeEulerAndSumFunction
        {
        public:
            ComputeEulerAndSumFunction(emitters::IRFunctionEmitter& function, llvm::Value* maxValue)
                : _maxValue(maxValue)
            {
                auto valueType = emitters::GetVariableType<ValueType>();
                _accumValueVar = function.Variable(valueType, "eulerSumAccumValue");
                _expFunc = function.GetModule().GetRuntime().GetExpFunction<ValueType>();
                Reset(function);
            }

            void Reset(emitters::IRFunctionEmitter& function)
            {
                function.Store(_accumValueVar, function.Literal<ValueType>(0));
            }

            llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x)
            {
                const auto plusFloat = emitters::TypedOperator::addFloat;
                const auto minusFloat = emitters::TypedOperator::subtractFloat;
                auto valueMinusMax = function.Operator(minusFloat, x, _maxValue);
                auto eulerVal = function.Call(_expFunc, { valueMinusMax });
                function.OperationAndUpdate(_accumValueVar, plusFloat, eulerVal);
                return eulerVal;
            }

            llvm::Value* GetEulerSum(emitters::IRFunctionEmitter& function) const
            {
                return function.Load(_accumValueVar);
            }

        private:
            llvm::Function* _expFunc;
            llvm::Value* _maxValue;
            llvm::Value* _accumValueVar;
        };

        template <typename ValueType>
        class NormalizeOutputFunction
        {
        public:
            NormalizeOutputFunction(emitters::IRFunctionEmitter& function, llvm::Value* sum)
                : _sum(sum)
            {
            }

            void Reset(emitters::IRFunctionEmitter& function)
            {
            }

            llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x)
            {
                const auto divideFloat = emitters::TypedOperator::divideFloat;
                return function.Operator(divideFloat, x, _sum);
            }

        public:
            llvm::Value* _sum;
        };

    } // end anonymous namespace

    template <typename ValueType>
    SoftmaxLayerNode<ValueType>::SoftmaxLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::SoftmaxLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<SoftmaxLayerNode<ValueType>, predictors::neural::SoftmaxLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    void SoftmaxLayerNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        llvm::Value* prevInputDimensionOffset = nullptr;
        llvm::Value* prevOutputDimensionOffset = nullptr;

        // Compute max value
        FindMaxFunction<ValueType> findMax(function);
        EmitComputeDimensionLoop(compiler, function, 0, this->GetInputMemoryLayout(), this->GetOutputMemoryLayout(), pInput, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, findMax);
        auto maxValue = findMax.GetMaxValue(function);

        // Compute sum and scale output
        ComputeEulerAndSumFunction<ValueType> computeEuler(function, maxValue);
        EmitComputeDimensionLoop(compiler, function, 0, this->GetInputMemoryLayout(), this->GetOutputMemoryLayout(), pInput, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, computeEuler);
        auto eulerSum = computeEuler.GetEulerSum(function);

        // normalize output
        NormalizeOutputFunction<ValueType> normalizeOutput(function, eulerSum);
        EmitComputeDimensionLoop(compiler, function, 0, this->GetOutputMemoryLayout(), pOutput, prevOutputDimensionOffset, normalizeOutput);
    }

    template <typename ValueType>
    template <typename FunctionType>
    void SoftmaxLayerNode<ValueType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                               size_t dimension,
                                                               const PortMemoryLayout& inputLayout,
                                                               const PortMemoryLayout& outputLayout,
                                                               llvm::Value* pInput,
                                                               llvm::Value* pOutput,
                                                               llvm::Value* prevInputDimensionOffset,
                                                               llvm::Value* prevOutputDimensionOffset,
                                                               FunctionType& f) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        const auto plus = emitters::TypedOperator::add;
        const auto times = emitters::TypedOperator::multiply;

        const auto numDimensions = this->NumInputDimensions();
        auto&& inputStride = inputLayout.stride;
        auto&& inputOffset = inputLayout.offset;
        auto&& inputSize = inputLayout.size;

        auto&& outputStride = outputLayout.stride;
        auto&& outputOffset = outputLayout.offset;

        auto loop = function.ForLoop();
        loop.Begin(inputSize[dimension]);
        {
            auto loopIndex = loop.LoadIterationVariable();

            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            llvm::Value* thisInputDimensionInternalOffset = function.Operator(plus, loopIndex, function.Literal<int>(inputOffset[dimension]));
            llvm::Value* thisOutputDimensionInternalOffset = function.Operator(plus, loopIndex, function.Literal<int>(outputOffset[dimension]));

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            llvm::Value* thisInputDimensionOffset = nullptr;
            llvm::Value* thisOutputDimensionOffset = nullptr;
            if (dimension == 0)
            {
                assert(prevInputDimensionOffset == nullptr);
                assert(prevOutputDimensionOffset == nullptr);
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                auto scaledInputDimensionOffset = function.Operator(times, prevInputDimensionOffset, function.Literal<int>(inputStride[dimension]));
                thisInputDimensionOffset = function.Operator(plus, scaledInputDimensionOffset, thisInputDimensionInternalOffset);

                auto scaledOutputDimensionOffset = function.Operator(times, prevOutputDimensionOffset, function.Literal<int>(outputStride[dimension]));
                thisOutputDimensionOffset = function.Operator(plus, scaledOutputDimensionOffset, thisOutputDimensionInternalOffset);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, inputLayout, outputLayout, pInput, pOutput, thisInputDimensionOffset, thisOutputDimensionOffset, f);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto inputValue = function.ValueAt(pInput, thisInputDimensionOffset);
                auto outputValue = f.Compile(function, inputValue);
                if (outputValue != nullptr)
                {
                    function.SetValueAt(pOutput, thisOutputDimensionOffset, outputValue);
                }
            }
        }
        loop.End();
    }

    // In-place version
    template <typename ValueType>
    template <typename FunctionType>
    void SoftmaxLayerNode<ValueType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                               size_t dimension,
                                                               const PortMemoryLayout& inputLayout,
                                                               llvm::Value* pInput,
                                                               llvm::Value* prevInputDimensionOffset,
                                                               FunctionType& f) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous

        const auto plus = emitters::TypedOperator::add;
        const auto times = emitters::TypedOperator::multiply;

        const auto numDimensions = this->NumInputDimensions();
        auto&& inputStride = inputLayout.stride;
        auto&& inputOffset = inputLayout.offset;
        auto&& inputSize = inputLayout.size;

        auto loop = function.ForLoop();
        loop.Begin(inputSize[dimension]);
        {
            auto loopIndex = loop.LoadIterationVariable();

            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            llvm::Value* thisInputDimensionInternalOffset = function.Operator(plus, loopIndex, function.Literal<int>(inputOffset[dimension]));

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            llvm::Value* thisInputDimensionOffset = nullptr;
            if (dimension == 0)
            {
                assert(prevInputDimensionOffset == nullptr);
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
            }
            else
            {
                auto scaledInputDimensionOffset = function.Operator(times, prevInputDimensionOffset, function.Literal<int>(inputStride[dimension]));
                thisInputDimensionOffset = function.Operator(plus, scaledInputDimensionOffset, thisInputDimensionInternalOffset);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, inputLayout, pInput, thisInputDimensionOffset, f);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto inputValue = function.ValueAt(pInput, thisInputDimensionOffset);
                auto outputValue = f.Compile(function, inputValue);
                if (outputValue != nullptr)
                {
                    function.SetValueAt(pInput, thisInputDimensionOffset, outputValue);
                }
            }
        }
        loop.End();
    }

    template <typename ValueType>
    size_t SoftmaxLayerNode<ValueType>::NumElements(const Shape& size)
    {
        size_t result = 1;
        for (auto s : size)
        {
            result *= s;
        }
        return result;
    }

    // Explicit specialization
    template class SoftmaxLayerNode<float>;
    template class SoftmaxLayerNode<double>;
} // nodes
} // ell
