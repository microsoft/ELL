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

            emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x)
            {
                function.If(emitters::TypedComparison::greaterThanFloat, x, function.Load(_accumValueVar), [this, x](emitters::IRFunctionEmitter& function) {
                    function.Store(_accumValueVar, x);
                });

                return nullptr;
            }

            emitters::LLVMValue GetMaxValue(emitters::IRFunctionEmitter& function) const
            {
                return function.Load(_accumValueVar);
            }

        private:
            emitters::LLVMValue _accumValueVar;
        };

        template <typename ValueType>
        class ComputeEulerAndSumFunction
        {
        public:
            ComputeEulerAndSumFunction(emitters::IRFunctionEmitter& function, emitters::LLVMValue maxValue)
                : _maxValue(maxValue)
            {
                auto valueType = emitters::GetVariableType<ValueType>();
                _accumValueVar = function.Variable(valueType, "eulerSumAccumValue");
                _expFunc = function.GetModule().GetRuntime().GetExpFunction<ValueType>();
                Reset(function);
            }

            void Reset(emitters::IRFunctionEmitter& function)
            {
                function.StoreZero(_accumValueVar);
            }

            emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x)
            {
                const auto plusFloat = emitters::TypedOperator::addFloat;
                const auto minusFloat = emitters::TypedOperator::subtractFloat;
                auto valueMinusMax = function.Operator(minusFloat, x, _maxValue);
                auto eulerVal = function.Call(_expFunc, { valueMinusMax });
                function.OperationAndUpdate(_accumValueVar, plusFloat, eulerVal);
                return eulerVal;
            }

            emitters::LLVMValue GetEulerSum(emitters::IRFunctionEmitter& function) const
            {
                return function.Load(_accumValueVar);
            }

        private:
            emitters::LLVMFunction _expFunc;
            emitters::LLVMValue _maxValue;
            emitters::LLVMValue _accumValueVar;
        };

        template <typename ValueType>
        class NormalizeOutputFunction
        {
        public:
            NormalizeOutputFunction(emitters::IRFunctionEmitter& function, emitters::LLVMValue sum)
                : _sum(sum)
            {
            }

            void Reset(emitters::IRFunctionEmitter& function)
            {
            }

            emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x)
            {
                const auto divideFloat = emitters::TypedOperator::divideFloat;
                return function.Operator(divideFloat, x, _sum);
            }

        public:
            emitters::LLVMValue _sum;
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
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(output);

        emitters::LLVMValue prevInputDimensionOffset = nullptr;
        emitters::LLVMValue prevOutputDimensionOffset = nullptr;

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
                                                               const model::PortMemoryLayout& inputLayout,
                                                               const model::PortMemoryLayout& outputLayout,
                                                               emitters::LLVMValue pInput,
                                                               emitters::LLVMValue pOutput,
                                                               emitters::LLVMValue prevInputDimensionOffset,
                                                               emitters::LLVMValue prevOutputDimensionOffset,
                                                               FunctionType& f) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        const auto numDimensions = this->NumInputDimensions();
        auto&& inputStride = inputLayout.GetStride();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();

        auto&& outputStride = outputLayout.GetStride();
        auto&& outputOffset = outputLayout.GetOffset();

        function.For(inputSize[dimension], [dimension, numDimensions, inputStride, inputOffset, inputLayout, outputStride, outputOffset, outputLayout, pInput, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, &f, &compiler, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            auto loopIndex = function.LocalScalar(i);
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            auto thisInputDimensionOffset = function.LocalScalar();
            auto thisOutputDimensionOffset = function.LocalScalar();
            if (dimension == 0)
            {
                assert(prevInputDimensionOffset == nullptr);
                assert(prevOutputDimensionOffset == nullptr);
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                auto scaledInputDimensionOffset = prevInputDimensionOffset * function.LocalScalar<int>(inputStride[dimension]);
                thisInputDimensionOffset = scaledInputDimensionOffset + thisInputDimensionInternalOffset;

                auto scaledOutputDimensionOffset = prevOutputDimensionOffset * function.LocalScalar<int>(outputStride[dimension]);
                thisOutputDimensionOffset = scaledOutputDimensionOffset + thisOutputDimensionInternalOffset;
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
        });
    }

    // In-place version
    template <typename ValueType>
    template <typename FunctionType>
    void SoftmaxLayerNode<ValueType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                               size_t dimension,
                                                               const model::PortMemoryLayout& inputLayout,
                                                               emitters::LLVMValue pInput,
                                                               emitters::LLVMValue prevInputDimensionOffsetValue,
                                                               FunctionType& f) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        const auto numDimensions = this->NumInputDimensions();
        auto&& inputStride = inputLayout.GetStride();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();

        auto input = function.LocalArray(pInput);
        auto prevInputDimensionOffset = function.LocalScalar(prevInputDimensionOffsetValue);
        function.For(inputSize[dimension], [dimension, numDimensions, inputOffset, inputStride, inputLayout, input, prevInputDimensionOffset, &f, &compiler, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            auto loopIndex = function.LocalScalar(i);
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            auto thisInputDimensionOffset = function.LocalScalar();
            if (dimension == 0)
            {
                assert(!prevInputDimensionOffset.IsValid());
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
            }
            else
            {
                auto scaledInputDimensionOffset = prevInputDimensionOffset * inputStride[dimension];
                thisInputDimensionOffset = scaledInputDimensionOffset + thisInputDimensionInternalOffset;
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, inputLayout, input, thisInputDimensionOffset, f);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                emitters::IRLocalScalar inputValue = input[thisInputDimensionOffset];
                auto outputValue = f.Compile(function, inputValue);
                if (outputValue != nullptr)
                {
                    input[thisInputDimensionOffset] = outputValue;
                }
            }
        });
    }

    template <typename ValueType>
    void SoftmaxLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<SoftmaxLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    // Explicit specialization
    template class SoftmaxLayerNode<float>;
    template class SoftmaxLayerNode<double>;
} // nodes
} // ell
