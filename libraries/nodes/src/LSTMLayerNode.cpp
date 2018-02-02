////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMLayerNode.cpp (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LSTMLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "CompiledActivationFunctions.h" // For sigmoid and tanh
#include "ConstantNode.h"
#include "MatrixVectorMultiplyNode.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    LSTMLayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::LSTMLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
        : BaseType(input, layer)
    {
        const auto& layerParameters = layer.GetLayerParameters();
        if (HasPadding(layerParameters.inputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "LSTMLayerNode does not currently support inputs with padding");
        }

        if (HasPadding(layerParameters.outputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "LSTMLayerNode does not currently support outputs with padding");
        }
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    bool LSTMLayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Transform weights and bias members into constant nodes
        const auto& inputWeights = this->_layer.GetInputWeights();
        const auto& forgetMeWeights = this->_layer.GetForgetMeWeights();
        const auto& candidateWeights = this->_layer.GetCandidateWeights();
        const auto& outputWeights = this->_layer.GetOutputWeights();
        const auto& inputBias = this->_layer.GetInputBias();
        const auto& forgetMeBias = this->_layer.GetForgetMeBias();
        const auto& candidateBias = this->_layer.GetCandidateBias();
        const auto& outputBias = this->_layer.GetOutputBias();

        auto inputWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(inputWeights.ToArray());
        auto forgetMeWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(forgetMeWeights.ToArray());
        auto candidateWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(candidateWeights.ToArray());
        auto outputWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(outputWeights.ToArray());

        auto inputBiasNode = transformer.AddNode<ConstantNode<ValueType>>(inputBias.ToArray());
        auto forgetMeBiasNode = transformer.AddNode<ConstantNode<ValueType>>(forgetMeBias.ToArray());
        auto candidateBiasNode = transformer.AddNode<ConstantNode<ValueType>>(candidateBias.ToArray());
        auto outputBiasNode = transformer.AddNode<ConstantNode<ValueType>>(outputBias.ToArray());

        using ComputeNodeType = LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>;
        auto lstmNode = transformer.AddNode<ComputeNodeType>(newInput,
                                                             inputWeightsNode->output,
                                                             forgetMeWeightsNode->output,
                                                             candidateWeightsNode->output,
                                                             outputWeightsNode->output,
                                                             inputBiasNode->output,
                                                             forgetMeBiasNode->output,
                                                             candidateBiasNode->output,
                                                             outputBiasNode->output,
                                                             this->GetInputMemoryLayout(),
                                                             this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, lstmNode->output);
        return true;
    }

    //
    // LSTMNode
    //
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::LSTMNode()
        : CompilableNode({ &_input, &_inputWeights, &_forgetMeWeights, &_candidateWeights, &_outputWeights, &_inputBias, &_forgetMeBias, &_candidateBias, &_outputBias },
                         { &_output })
        , _input(this, {}, defaultInputPortName)
        , _inputWeights(this, {}, inputWeightsPortName)
        , _forgetMeWeights(this, {}, forgetMeWeightsPortName)
        , _candidateWeights(this, {}, candidateWeightsPortName)
        , _outputWeights(this, {}, outputWeightsPortName)
        , _inputBias(this, {}, inputBiasPortName)
        , _forgetMeBias(this, {}, forgetMeBiasPortName)
        , _candidateBias(this, {}, candidateBiasPortName)
        , _outputBias(this, {}, outputBiasPortName)
        , _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::LSTMNode(const model::PortElements<ValueType>& input,
                                                                                           const model::PortElements<ValueType>& inputWeights,
                                                                                           const model::PortElements<ValueType>& forgetMeWeights,
                                                                                           const model::PortElements<ValueType>& candidateWeights,
                                                                                           const model::PortElements<ValueType>& outputWeights,
                                                                                           const model::PortElements<ValueType>& inputBias,
                                                                                           const model::PortElements<ValueType>& forgetMeBias,
                                                                                           const model::PortElements<ValueType>& candidateBias,
                                                                                           const model::PortElements<ValueType>& outputBias,
                                                                                           const model::PortMemoryLayout& inputMemoryLayout,
                                                                                           const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_inputWeights, &_forgetMeWeights, &_candidateWeights, &_outputWeights, &_inputBias, &_forgetMeBias, &_candidateBias, &_outputBias },
                         { &_output })
        , _input(this, input, defaultInputPortName)
        , _inputWeights(this, inputWeights, inputWeightsPortName)
        , _forgetMeWeights(this, forgetMeWeights, forgetMeWeightsPortName)
        , _candidateWeights(this, candidateWeights, candidateWeightsPortName)
        , _outputWeights(this, outputWeights, outputWeightsPortName)
        , _inputBias(this, inputBias, inputBiasPortName)
        , _forgetMeBias(this, forgetMeBias, forgetMeBiasPortName)
        , _candidateBias(this, candidateBias, candidateBiasPortName)
        , _outputBias(this, outputBias, outputBiasPortName)
        , _output(this, defaultOutputPortName, inputBias.Size())
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newInputWeights = transformer.TransformPortElements(_inputWeights.GetPortElements());
        auto newForgetMeWeights = transformer.TransformPortElements(_forgetMeWeights.GetPortElements());
        auto newCandidateWeights = transformer.TransformPortElements(_candidateWeights.GetPortElements());
        auto newOutputWeights = transformer.TransformPortElements(_outputWeights.GetPortElements());
        auto newInputBias = transformer.TransformPortElements(_inputBias.GetPortElements());
        auto newForgetMeBias = transformer.TransformPortElements(_forgetMeBias.GetPortElements());
        auto newCandidateBias = transformer.TransformPortElements(_candidateBias.GetPortElements());
        auto newOutputBias = transformer.TransformPortElements(_outputBias.GetPortElements());
        auto newNode = transformer.AddNode<LSTMNode>(newInput, newInputWeights, newForgetMeWeights, newCandidateWeights, newOutputWeights, newInputBias, newForgetMeBias, newCandidateBias, newOutputBias, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "LSTMNode does not currently compute");
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* data, size_t dataLength)
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

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    template <typename ActivationType>
    void LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength)
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

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void LSTMNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;

        const size_t inputSize = this->input.Size();
        const size_t hiddenSize = this->inputBias.Size();

        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);
        RecurrentActivationFunctionType<ValueType> recurrentLayerActivationFunction;
        auto recurrentActivationFunction = GetNodeActivationFunction(recurrentLayerActivationFunction);

        // Global state (in addition to output)
        llvm::GlobalVariable* ctActual = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), "ctActual", hiddenSize);

        // Get LLVM references for all node inputs
        llvm::Value* input = compiler.EnsurePortEmitted(this->input);
        llvm::Value* inputWeights = compiler.EnsurePortEmitted(this->inputWeights);
        llvm::Value* forgetMeWeights = compiler.EnsurePortEmitted(this->forgetMeWeights);
        llvm::Value* candidateWeights = compiler.EnsurePortEmitted(this->candidateWeights);
        llvm::Value* outputWeights = compiler.EnsurePortEmitted(this->outputWeights);

        llvm::Value* inputBias = compiler.EnsurePortEmitted(this->inputBias);
        llvm::Value* forgetMeBias = compiler.EnsurePortEmitted(this->forgetMeBias);
        llvm::Value* candidateBias = compiler.EnsurePortEmitted(this->candidateBias);
        llvm::Value* outputBias = compiler.EnsurePortEmitted(this->outputBias);

        // Get LLVM reference for node output
        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

        // The node's output is the same as the hidden state --- just make an alias so the code looks nicer
        auto& hiddenState = output;

        // Allocate local variables
        llvm::AllocaInst* inputPlusHidden = function.Variable(emitters::GetVariableType<ValueType>(), inputSize + hiddenSize);
        llvm::AllocaInst* ft = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);
        llvm::AllocaInst* it = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);
        llvm::AllocaInst* ctNew = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);
        llvm::AllocaInst* ot = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);

        // Concatenate input and hidden state into combined [Xt, Ht-1]
        function.MemoryCopy<ValueType>(input, inputPlusHidden, inputSize);
        function.MemoryCopy<ValueType>(hiddenState, 0, inputPlusHidden, inputSize, hiddenSize);

        // ft = recurrentFunction(Wf * [Xt, Ht-1] + Bf)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(forgetMeBias, ft, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), forgetMeWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ft, 1);
        ApplyActivation(function, recurrentActivationFunction, ft, hiddenSize);

        // it = recurrentFunction(Wi * [Xt, Ht-1] + Bi)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(inputBias, it, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), inputWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), it, 1);
        ApplyActivation(function, recurrentActivationFunction, it, hiddenSize);

        // Ct~ = activationFunction(Wc * [Xt, Ht-1] + Bc)  (where activationFunction is usually tanh)
        function.MemoryCopy<ValueType>(candidateBias, ctNew, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), candidateWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ctNew, 1);
        ApplyActivation(function, activationFunction, ctNew, hiddenSize);

        // Ct = ft * Ct-1 + it * Ct~
        auto ctActualLoop = function.ForLoop();
        ctActualLoop.Begin(hiddenSize);
        {
            auto index = ctActualLoop.LoadIterationVariable();
            auto ctActualVal = function.ValueAt(ctActual, index);
            auto ftVal = function.ValueAt(ft, index);
            auto itVal = function.ValueAt(it, index);
            auto ctNewVal = function.ValueAt(ctNew, index);

            auto ftCt = function.Operator(timesFloat, ftVal, ctActualVal);
            auto itctNew = function.Operator(timesFloat, itVal, ctNewVal);

            auto result = function.Operator(plusFloat, ftCt, itctNew);
            function.SetValueAt(ctActual, index, result);
        }
        ctActualLoop.End();

        // ot = recurrentFunction(Wo * [Xt, Ht-1] + Bo)
        function.MemoryCopy<ValueType>(outputBias, ot, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), outputWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ot, 1);
        ApplyActivation(function, recurrentActivationFunction, ot, hiddenSize);

        // compute the hidden layer and copy into state vector
        // ht = ot * activationFunction(Ct)  (where activationFunction is usually tanh)
        ApplyActivation(function, activationFunction, ctActual, hiddenSize);

        auto outputLoop = function.ForLoop();
        outputLoop.Begin(hiddenSize);
        {
            auto index = outputLoop.LoadIterationVariable();
            auto otVal = function.ValueAt(ot, index);
            auto ctVal = function.ValueAt(ctActual, index);
            auto result = function.Operator(timesFloat, ctVal, otVal);
            function.SetValueAt(hiddenState, index, result);
        }
        outputLoop.End();
        // output <- hiddenState (no-op, since output and hidden state are aliases)
    }

    // Explicit specialization
#define INSTANTIATE_LSTM(activation1, activation2) \
    template class LSTMLayerNode<float, activation1, activation2>; \
    template class LSTMLayerNode<double, activation1, activation2>; \
    template class LSTMNode<float, activation1, activation2>; \
    template class LSTMNode<double, activation1, activation2>

    INSTANTIATE_LSTM(predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_LSTM(predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation);

    INSTANTIATE_LSTM(predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::ReLUActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_LSTM(predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::ReLUActivation, predictors::neural::TanhActivation);

    INSTANTIATE_LSTM(predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_LSTM(predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::SigmoidActivation, predictors::neural::TanhActivation);

    INSTANTIATE_LSTM(predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::TanhActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_LSTM(predictors::neural::TanhActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_LSTM(predictors::neural::TanhActivation, predictors::neural::TanhActivation);
} // nodes
} // ell
