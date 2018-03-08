////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SimpleConvolutionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleConvolutionNode.h"
#include "ConstantNode.h"

// math
#include "Matrix.h"

namespace ell
{
namespace nodes
{
    namespace
    {
        size_t GetOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }
    } // end anonymous namespace

    //
    // SimpleConvolutionNode
    //

    template <typename ValueType>
    SimpleConvolutionNode<ValueType>::SimpleConvolutionNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    SimpleConvolutionNode<ValueType>::SimpleConvolutionNode(const model::PortElements<ValueType>& input,
                                                            const model::PortMemoryLayout& inputMemoryLayout,
                                                            const model::PortMemoryLayout& outputMemoryLayout,
                                                            const ConstTensorReferenceType& filterWeights,
                                                            const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                            const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                            const predictors::neural::PaddingParameters& outputPaddingParameters)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterWeights(filterWeights), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SimpleConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, _outputMemoryLayout, _filterWeights, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    bool SimpleConvolutionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // (row, column), channel order:
        const auto& weightsMatrix = _filterWeights.ReferenceAsMatrix();
        const auto weightsValues = weightsMatrix.ToArray();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto convNode = transformer.AddNode<SimpleConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, _outputMemoryLayout, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
        transformer.MapNodeOutput(this->output, convNode->output);
        return true;
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    //
    // SimpleConvolutionComputeNode
    //

    template <typename ValueType>
    SimpleConvolutionComputeNode<ValueType>::SimpleConvolutionComputeNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _filterWeights(this, {}, filterWeightsPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    SimpleConvolutionComputeNode<ValueType>::SimpleConvolutionComputeNode(const model::PortElements<ValueType>& input,
                                                                          const model::PortElements<ValueType>& filterWeights,
                                                                          const model::PortMemoryLayout& inputMemoryLayout,
                                                                          const model::PortMemoryLayout& outputMemoryLayout,
                                                                          const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                                          const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                                          const predictors::neural::PaddingParameters& outputPaddingParameters)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
    }

    template <typename ValueType>
    void SimpleConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<SimpleConvolutionComputeNode<ValueType>>(newInput, newFilterWeights, _inputMemoryLayout, _outputMemoryLayout, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void SimpleConvolutionComputeNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    // Terminology:
    // fw: filter width
    // d: # input channels
    // f: # filters (== output channels)

    template <typename ValueType>
    void SimpleConvolutionComputeNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // input is a d x (w+2p) x (h+2p) array
        // reshaped, it's a d*(w+2p)) x (h+2p) array == d*(w+k-1) x (h+k-1)
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);

        // weights is f x k x k x d array
        // reshaped, it's (f*k) x (k*d) or f x k x (k*d)
        llvm::Value* pWeights = compiler.EnsurePortEmitted(this->filterWeights);

        // output is a (w+2p) x (h+2p) x f array
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        // Model parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const size_t padding = inputLayout.GetOffset(0);
        DEBUG_USED(padding);
        assert((padding == filterWidth / 2) && "Padding must be filterWidth/2");

        // output data parameters
        const size_t outputRows = outputLayout.GetActiveSize(0);
        const size_t outputColumns = outputLayout.GetActiveSize(1);
        const size_t numFilters = outputLayout.GetActiveSize(2);

        auto inputMemoryIncrements = inputLayout.GetCumulativeIncrement();
        auto outputMemoryIncrements = outputLayout.GetCumulativeIncrement();

        // For each filter
        function.For(numFilters, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
            auto filterIndex = function.LocalScalar(loopIndex1);

            // For each output row
            function.For(outputRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                auto outputRow = function.LocalScalar(loopIndex2);

                // For each output column
                function.For(outputColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex3) {
                    auto outputColumn = function.LocalScalar(loopIndex3);

                    auto outputOffset = (outputRow * function.LocalScalar(outputMemoryIncrements[0])) +
                                        (outputColumn * function.LocalScalar(outputMemoryIncrements[1])) +
                                        (filterIndex * function.LocalScalar(outputMemoryIncrements[2]));
                    auto outputPtr = function.PointerOffset(pOutput, outputOffset);

                    // The filters are typically small, so we unroll the loops here
                    for (size_t windowRow = 0; windowRow < filterWidth; ++windowRow)
                    {
                        // Note: if the memory storage from consecutive columns is contiguous, we can process them together and avoid a loop
                        const bool canCombineColumns = _inputMemoryLayout.GetActiveSize(1) == _inputMemoryLayout.GetStride(1);
                        if (canCombineColumns)
                        {
                            auto inputOffset = ((outputRow + function.LocalScalar<int>(windowRow)) * function.LocalScalar<int>(inputMemoryIncrements[0])) +
                                               (outputColumn * function.LocalScalar<int>(inputMemoryIncrements[1]));
                            auto imageRow = function.PointerOffset(pInput, inputOffset);
                            auto filterOffset = function.LocalScalar<int>(inputDepth * (filterWidth * windowRow)) +
                                                (filterIndex * function.LocalScalar<int>(filterWidth * filterWidth * inputDepth));
                            auto filterRow = function.PointerOffset(pWeights, filterOffset);
                            auto val = function.LocalScalar(function.DotProduct(filterWidth * inputDepth, imageRow, filterRow));
                            if (windowRow == 0)
                            {
                                function.Store(outputPtr, val);
                            }
                            else
                            {
                                function.Store(outputPtr, function.LocalScalar(function.Load(outputPtr)) + val);
                            }
                        }
                        else
                        {
                            for (size_t windowColumn = 0; windowColumn < filterWidth; ++windowColumn)
                            {
                                // I[r+wc, c+wc]
                                auto inputOffset = ((outputRow + function.LocalScalar<int>(windowRow)) * function.LocalScalar<int>(inputMemoryIncrements[0])) +
                                                   ((outputColumn + function.LocalScalar<int>(windowColumn)) * function.LocalScalar<int>(inputMemoryIncrements[1]));
                                auto imageRow = function.PointerOffset(pInput, inputOffset);
                                auto filterOffset = function.LocalScalar<int>(inputDepth * (filterWidth * windowRow + windowColumn)) +
                                                    (filterIndex * function.LocalScalar<int>(filterWidth * filterWidth * inputDepth));
                                auto filterRow = function.PointerOffset(pWeights, filterOffset);
                                auto val = function.LocalScalar(function.DotProduct(inputDepth, imageRow, filterRow));
                                if (windowRow == 0 && windowColumn == 0)
                                {
                                    function.Store(outputPtr, val);
                                }
                                else
                                {
                                    function.Store(outputPtr, function.LocalScalar(function.Load(outputPtr)) + val);
                                }
                            }
                        }
                    }
                }); // End outputColumns loop
            }); // End outputRows loop
        }); // End numFilters loop
    }

    // Explicit specializations
    template class SimpleConvolutionNode<float>;
    template class SimpleConvolutionNode<double>;
} // nodes
} // ell
