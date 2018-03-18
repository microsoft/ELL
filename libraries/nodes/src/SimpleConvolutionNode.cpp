////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SimpleConvolutionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleConvolutionNode.h"
#include "ConstantNode.h"

// dsp
#include "Convolution.h"

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

        //
        // Low-level code-generation
        //
        void EmitSimpleConvolutionCode(emitters::IRFunctionEmitter& function, llvm::Value* input, llvm::Value* filterWeights, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, int filterSize, int stride, llvm::Value* result)
        {
            // input is a d x (w+2p) x (h+2p) array
            // reshaped, it's a d*(w+2p)) x (h+2p) array == d*(w+k-1) x (h+k-1)

            // filterWeights is f x k x k x d array
            // reshaped, it's (f*k) x (k*d) or f x k x (k*d)

            // output is a (w+2p) x (h+2p) x f array

            // Model parameters
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto inputPadding = inputLayout.GetOffset(0);
            DEBUG_USED(inputPadding);
            assert((inputPadding == filterSize / 2) && "Input padding must be filterSize/2");

            // output data parameters
            const auto outputRows = outputLayout.GetActiveSize(0);
            const auto outputColumns = outputLayout.GetActiveSize(1);
            const auto numFilters = outputLayout.GetActiveSize(2);

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
                        auto outputPtr = function.PointerOffset(result, outputOffset);

                        // The filters are typically small, so we unroll the loops here
                        for (int windowRow = 0; windowRow < filterSize; ++windowRow)
                        {
                            // Note: if the memory storage from consecutive columns is contiguous, we can process them together and avoid a loop
                            const bool canCombineColumns = (inputLayout.GetActiveSize(1) == inputLayout.GetStride(1)) && (stride == 1);
                            if (canCombineColumns)
                            {
                                auto inputOffset = ((outputRow + function.LocalScalar<int>(windowRow)) * function.LocalScalar<int>(inputMemoryIncrements[0])) +
                                                   (outputColumn * function.LocalScalar<int>(inputMemoryIncrements[1]));
                                auto imageRow = function.PointerOffset(input, inputOffset);
                                auto filterOffset = function.LocalScalar<int>(inputDepth * (filterSize * windowRow)) +
                                                    (filterIndex * function.LocalScalar<int>(filterSize * filterSize * inputDepth));
                                auto filterRow = function.PointerOffset(filterWeights, filterOffset);
                                auto val = function.LocalScalar(function.DotProduct(filterSize * inputDepth, imageRow, filterRow));
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
                                for (int windowColumn = 0; windowColumn < filterSize; ++windowColumn)
                                {
                                    // I[r+wc, c+wc]
                                    auto inputRow = outputRow * stride;
                                    auto inputColumn = outputColumn * stride;
                                    auto inputOffset = ((inputRow + function.LocalScalar<int>(windowRow)) * function.LocalScalar<int>(inputMemoryIncrements[0])) +
                                                       ((inputColumn + function.LocalScalar<int>(windowColumn)) * function.LocalScalar<int>(inputMemoryIncrements[1]));
                                    auto imageRow = function.PointerOffset(input, inputOffset);
                                    auto filterOffset = function.LocalScalar<int>(inputDepth * (filterSize * windowRow + windowColumn)) +
                                                        (filterIndex * function.LocalScalar<int>(filterSize * filterSize * inputDepth));
                                    auto filterRow = function.PointerOffset(filterWeights, filterOffset);
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
                                                            size_t stride)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterWeights(filterWeights), _stride(static_cast<int>(stride))
    {
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SimpleConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, _outputMemoryLayout, _filterWeights, _stride);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    bool SimpleConvolutionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // (row, column), channel order:
        const auto& weightsMatrix = _filterWeights.ReferenceAsMatrix();
        const auto weightsValues = weightsMatrix.ToArray();
        const int filterSize = _filterWeights.NumColumns();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto convNode = transformer.AddNode<SimpleConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, _outputMemoryLayout, filterSize, _stride);
        transformer.MapNodeOutput(this->output, convNode->output);
        return true;
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << _outputMemoryLayout;
        archiver["stride"] << _stride;
        math::TensorArchiver::Write(_filterWeights, "weights", archiver);
    }

    template <typename ValueType>
    void SimpleConvolutionNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        archiver["outputLayout"] >> _outputMemoryLayout;
        archiver["stride"] >> _stride;
        math::TensorArchiver::Read(_filterWeights, "weights", archiver);
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
                                                                          int filterSize,
                                                                          int stride)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterSize(filterSize), _stride(stride)
    {
    }

    template <typename ValueType>
    void SimpleConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<SimpleConvolutionComputeNode<ValueType>>(newInput, newFilterWeights, _inputMemoryLayout, _outputMemoryLayout, _filterSize, _stride);
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
        const auto inputPadding = inputLayout.GetOffset(0);
        DEBUG_USED(inputPadding);
        assert((inputPadding == _filterSize / 2) && "Input padding must be filterSize/2");

        EmitSimpleConvolutionCode(function, pInput, pWeights, inputLayout, outputLayout, _filterSize, _stride, pOutput);
    }

    // Explicit specializations
    template class SimpleConvolutionNode<float>;
    template class SimpleConvolutionNode<double>;
} // nodes
} // ell
