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

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType, template <typename> class ActivationFunctionType>
    RecurrentLayerNode<ValueType, ActivationFunctionType>::RecurrentLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>& layer)
        : NeuralNetworkLayerNode<RecurrentLayerNode<ValueType, ActivationFunctionType>, predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>, ValueType>(input, layer)
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

    template <typename ValueType, template <typename> class ActivationFunctionType>
    bool RecurrentLayerNode<ValueType, ActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Transform weights and bias members into constant nodes
        const auto& hiddenWeights = this->_layer.GetHiddenWeights();
        const auto& hiddenBias = this->_layer.GetHiddenBias();

        auto hiddenWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenWeights.ToArray());
        auto hiddenBiasNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenBias.ToArray());
        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);
        auto recurrentNode = transformer.AddNode<RecurrentNode<ValueType, ActivationFunctionType>>(newInput,
                                                                                                   hiddenWeightsNode->output,
                                                                                                   hiddenBiasNode->output,
                                                                                                   this->GetInputMemoryLayout(),
                                                                                                   this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, recurrentNode->output);
        return true;
    }

    //
    // RecurrentNode
    //
    template <typename ValueType, template <typename> class ActivationFunctionType>
    RecurrentNode<ValueType, ActivationFunctionType>::RecurrentNode()
        : CompilableNode({ &_input, &_hiddenWeights, &_hiddenBias }, { &_output }), _input(this, {}, defaultInputPortName), _hiddenWeights(this, {}, hiddenWeightsPortName), _hiddenBias(this, {}, hiddenBiasPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    RecurrentNode<ValueType, ActivationFunctionType>::RecurrentNode(const model::PortElements<ValueType>& input,
                                                                    const model::PortElements<ValueType>& hiddenWeights,
                                                                    const model::PortElements<ValueType>& hiddenBias,
                                                                    const model::PortMemoryLayout& inputMemoryLayout,
                                                                    const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_hiddenWeights, &_hiddenBias }, { &_output }), _input(this, input, defaultInputPortName), _hiddenWeights(this, hiddenWeights, hiddenWeightsPortName), _hiddenBias(this, hiddenBias, hiddenBiasPortName), _output(this, defaultOutputPortName, hiddenBias.Size())
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    void RecurrentNode<ValueType, ActivationFunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newHiddenWeights = transformer.TransformPortElements(_hiddenWeights.GetPortElements());
        auto newHiddenBias = transformer.TransformPortElements(_hiddenBias.GetPortElements());
        auto newNode = transformer.AddNode<RecurrentNode>(newInput, newHiddenWeights, newHiddenBias, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    void RecurrentNode<ValueType, ActivationFunctionType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "RecurrentNode does not currently compute");
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    template <typename ActivationType>
    void RecurrentNode<ValueType, ActivationFunctionType>::ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength)
    {
        auto forLoop = function.ForLoop();
        forLoop.Begin(dataLength);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* inputValue = function.ValueAt(data, i);
            llvm::Value* x = activationFunction.Compile(function, inputValue);
            function.SetValueAt(data, i, x);
        }
        forLoop.End();
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    void RecurrentNode<ValueType, ActivationFunctionType>::ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* data, size_t dataLength)
    {
        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto divideFloat = emitters::TypedOperator::divideFloat;
        auto expFunc = function.GetModule().GetRuntime().GetExpFunction<ValueType>();

        llvm::AllocaInst* sum = function.Variable(emitters::GetVariableType<ValueType>(), 1);
        function.SetValueAt(sum, 0, function.Literal<ValueType>(0.0));

        auto forLoop = function.ForLoop();
        forLoop.Begin(dataLength);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* inputValue = function.ValueAt(data, i);

            auto expInput = function.Call(expFunc, { inputValue });
            auto addToSum = function.Operator(plusFloat, function.ValueAt(sum, 0), expInput);
            function.SetValueAt(sum, 0, addToSum);
            function.SetValueAt(data, i, expInput);
        }
        forLoop.End();

        auto forLoop2 = function.ForLoop();
        forLoop2.Begin(dataLength);
        {
            auto i = forLoop2.LoadIterationVariable();
            llvm::Value* inputValue = function.ValueAt(data, i);
            auto expDivSum = function.Operator(divideFloat, inputValue, function.ValueAt(sum, 0));
            function.SetValueAt(data, i, expDivSum);
        }
        forLoop2.End();
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    void RecurrentNode<ValueType, ActivationFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const size_t inputSize = this->input.Size();
        const size_t hiddenSize = this->hiddenBias.Size();

        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);

        // Get LLVM references for all node inputs
        llvm::Value* input = compiler.EnsurePortEmitted(this->input);
        llvm::Value* hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        llvm::Value* hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

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

        ApplyActivation(function, activationFunction, hiddenState, hiddenSize);
        // output <- hiddenState (no-op, since output and hidden state are aliases)
    }

    // Explicit instantiations
    template class RecurrentLayerNode<float, predictors::neural::SigmoidActivation>;
    template class RecurrentLayerNode<double, predictors::neural::SigmoidActivation>;
    template class RecurrentLayerNode<float, predictors::neural::TanhActivation>;
    template class RecurrentLayerNode<double, predictors::neural::TanhActivation>;
    template class RecurrentNode<float, predictors::neural::SigmoidActivation>;
    template class RecurrentNode<double, predictors::neural::SigmoidActivation>;
    template class RecurrentNode<float, predictors::neural::TanhActivation>;
    template class RecurrentNode<double, predictors::neural::TanhActivation>;
} // nodes
} // ell
