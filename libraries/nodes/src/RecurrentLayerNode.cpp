////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RecurrentLayerNode.cpp (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RecurrentLayerNode.h"
#include "ActivationLayerNode.h" // For sigmoid and tanh
#include "BroadcastFunctionNode.h"
#include "CompiledActivationFunctions.h"
#include "ConstantNode.h"
#include "MatrixVectorMultiplyNode.h"
#include "SigmoidActivation.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    RecurrentLayerNode<ValueType>::RecurrentLayerNode(const model::OutputPort<ValueType>& input, const predictors::neural::RecurrentLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<RecurrentLayerNode<ValueType>, predictors::neural::RecurrentLayer<ValueType>, ValueType>(input, layer)
    {
        const auto& layerParameters = layer.GetLayerParameters();
        if (HasPadding(layerParameters.inputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "RecurrentLayerNode does not currently support inputs with padding");
        }

        if (HasPadding(layerParameters.outputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "RecurrentLayerNode does not currently support outputs with padding");
        }
    }

    template <typename ValueType>
    bool RecurrentLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->input);

        // Transform weights and bias members into constant nodes
        const auto& hiddenWeights = this->_layer.GetHiddenWeights();
        const auto& hiddenBias = this->_layer.GetHiddenBias();

        auto hiddenWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenWeights.ToArray());
        auto hiddenBiasNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenBias.ToArray());

        auto recurrentNode = transformer.AddNode<RecurrentNode<ValueType>>(newInput,
                                                                           hiddenWeightsNode->output,
                                                                           hiddenBiasNode->output,
                                                                           this->_layer.GetActivationFunction(),
                                                                           this->GetInputMemoryLayout(),
                                                                           this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, recurrentNode->output);
        return true;
    }

    template <typename ValueType>
    void RecurrentLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<RecurrentLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    //
    // RecurrentNode
    //
    template <typename ValueType>
    RecurrentNode<ValueType>::RecurrentNode()
        : CompilableNode({ &_input, &_hiddenWeights, &_hiddenBias }, { &_output }), _input(this, {}, defaultInputPortName), _hiddenWeights(this, {}, hiddenWeightsPortName), _hiddenBias(this, {}, hiddenBiasPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    RecurrentNode<ValueType>::RecurrentNode(const model::OutputPort<ValueType>& input,
                                                                    const model::OutputPort<ValueType>& hiddenWeights,
                                                                    const model::OutputPort<ValueType>& hiddenBias,
                                                                    const ActivationType& activation,
                                                                    const model::PortMemoryLayout& inputMemoryLayout,
                                                                    const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_hiddenWeights, &_hiddenBias }, { &_output }), _input(this, input, defaultInputPortName), _hiddenWeights(this, hiddenWeights, hiddenWeightsPortName), _hiddenBias(this, hiddenBias, hiddenBiasPortName), _output(this, defaultOutputPortName, hiddenBias.Size()),
        _activation(activation)
    {
    }

    template <typename ValueType>
    void RecurrentNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        const auto& newHiddenWeights = transformer.GetCorrespondingInputs(_hiddenWeights);
        const auto& newHiddenBias = transformer.GetCorrespondingInputs(_hiddenBias);
        auto newNode = transformer.AddNode<RecurrentNode>(newInput, newHiddenWeights, newHiddenBias, _activation, _inputMemoryLayout, GetOutputMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void RecurrentNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "RecurrentNode does not currently compute");
    }

    template <typename ValueType>
    void RecurrentNode<ValueType>::ApplyActivation(emitters::IRFunctionEmitter& function, emitters::LLVMValue data, size_t dataLength)
    {
        auto activationFunction = GetNodeActivationFunction(_activation);
        function.For(dataLength, [&](emitters::IRFunctionEmitter& function, auto i) {
            emitters::LLVMValue inputValue = function.ValueAt(data, i);
            emitters::LLVMValue x = activationFunction->Compile(function, inputValue);
            function.SetValueAt(data, i, x);
        });
    }

    template <typename ValueType>
    void RecurrentNode<ValueType>::ApplySoftmax(emitters::IRFunctionEmitter& function, emitters::LLVMValue dataValue, size_t dataLength)
    {
        auto data = function.LocalArray(dataValue);
        auto sum = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), 1));
        function.SetValueAt(sum, 0, function.Literal<ValueType>(0.0));

        function.For(dataLength, [sum, data](emitters::IRFunctionEmitter& function, auto i) {
            auto expInput = emitters::Exp(data[i]);
            sum[0] = sum[0] + expInput;
            data[i] = expInput;
        });

        function.For(dataLength, [sum, data](emitters::IRFunctionEmitter& function, auto i) {
            data[i] = data[i] / sum[0];
        });
    }

    template <typename ValueType>
    void RecurrentNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const size_t inputSize = this->input.Size();
        const size_t hiddenSize = this->hiddenBias.Size();

        // Get LLVM references for all node inputs
        emitters::LLVMValue input = compiler.EnsurePortEmitted(this->input);
        emitters::LLVMValue hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        emitters::LLVMValue hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        emitters::LLVMValue output = compiler.EnsurePortEmitted(this->output);

        // The node's output is the same as the hidden state --- just make an alias so the code looks nicer
        auto& hiddenState = output;

        // Allocate local variable
        llvm::AllocaInst* hiddenPlusInput = function.Variable(emitters::GetVariableType<ValueType>(), inputSize + hiddenSize);

        // concatenate hidden and input values
        function.MemoryCopy<ValueType>(input, hiddenPlusInput, inputSize);
        function.MemoryCopy<ValueType>(hiddenState, 0, hiddenPlusInput, inputSize, hiddenSize);

        // Ht = activationFunction(Wh * [Xt, Ht-1] + Bh)
        const auto m = hiddenSize;
        const auto n = inputSize + hiddenSize;
        function.MemoryCopy<ValueType>(hiddenBias, output, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(m, n, static_cast<ValueType>(1.0), hiddenWeights, n, hiddenPlusInput, 1, static_cast<ValueType>(1.0), output, 1);

        ApplyActivation(function, hiddenState, hiddenSize);
        // output <- hiddenState (no-op, since output and hidden state are aliases)
    }

    // Explicit instantiations
    template class RecurrentLayerNode<float>;
    template class RecurrentLayerNode<double>;
} // nodes
} // ell
