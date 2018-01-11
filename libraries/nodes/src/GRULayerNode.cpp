////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRULayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GRULayerNode.h"
#include "BroadcastFunctionNode.h"
#include "CompiledActivationFunctions.h"
#include "ConstantNode.h"
#include "HardSigmoidActivation.h"
#include "MatrixVectorMultiplyNode.h"
#include "SigmoidActivation.h"
#include "TanhActivation.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayerNode(const model::PortElements<ValueType>& input, const predictors::neural::GRULayer<ValueType>& layer)
        : BaseType(input, layer)
    {
        const auto& layerParameters = layer.GetLayerParameters();
        if (HasPadding(layerParameters.inputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "GRULayerNode does not currently support inputs with padding");
        }

        if (HasPadding(layerParameters.outputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "GRULayerNode does not currently support outputs with padding");
        }
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    bool GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Transform weights and bias members into constant nodes
        const auto& updateWeights = this->_layer.GetUpdateWeights();
        const auto& resetWeights = this->_layer.GetResetWeights();
        const auto& hiddenWeights = this->_layer.GetHiddenWeights();
        const auto& updateBias = this->_layer.GetUpdateBias();
        const auto& resetBias = this->_layer.GetResetBias();
        const auto& hiddenBias = this->_layer.GetHiddenBias();

        auto updateWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(updateWeights.ToArray());
        auto resetWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(resetWeights.ToArray());
        auto hiddenWeightsNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenWeights.ToArray());
        auto updateBiasNode = transformer.AddNode<ConstantNode<ValueType>>(updateBias.ToArray());
        auto resetBiasNode = transformer.AddNode<ConstantNode<ValueType>>(resetBias.ToArray());
        auto hiddenBiasNode = transformer.AddNode<ConstantNode<ValueType>>(hiddenBias.ToArray());

        auto gruNode = transformer.AddNode<GRUNode<ValueType,
                                                    ActivationFunctionType,
                                                    RecurrentActivationFunctionType>>(newInput,
                                                                                    updateWeightsNode->output,
                                                                                    resetWeightsNode->output,
                                                                                    hiddenWeightsNode->output,
                                                                                    updateBiasNode->output,
                                                                                    resetBiasNode->output,
                                                                                    hiddenBiasNode->output,
                                                                                    this->GetInputMemoryLayout(),
                                                                                    this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, gruNode->output);
        return true;
    }

    //
    // GRUNode
    //
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRUNode()
        : GRUNode({}, {}, {}, {}, {}, {}, {}, {}, {})
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRUNode(const model::PortElements<ValueType>& input,
                                                                                         const model::PortElements<ValueType>& updateWeights,
                                                                                         const model::PortElements<ValueType>& resetWeights,
                                                                                         const model::PortElements<ValueType>& hiddenWeights,
                                                                                         const model::PortElements<ValueType>& updateBias,
                                                                                         const model::PortElements<ValueType>& resetBias,
                                                                                         const model::PortElements<ValueType>& hiddenBias,
                                                                                         const model::PortMemoryLayout& inputMemoryLayout,
                                                                                         const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_updateWeights, &_resetWeights, &_hiddenWeights, &_updateBias, &_resetBias, &_hiddenBias },
                         { &_output }),
          _input(this, input, defaultInputPortName),
          _updateWeights(this, updateWeights, updateWeightsPortName),
          _resetWeights(this, resetWeights, resetWeightsPortName),
          _hiddenWeights(this, hiddenWeights, hiddenWeightsPortName),
          _updateBias(this, updateBias, updateBiasPortName),
          _resetBias(this, resetBias, resetBiasPortName),
          _hiddenBias(this, hiddenBias, hiddenBiasPortName),
          _output(this, defaultOutputPortName, updateBias.Size())
    {
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newUpdateWeights = transformer.TransformPortElements(_updateWeights.GetPortElements());
        auto newResetWeights = transformer.TransformPortElements(_resetWeights.GetPortElements());
        auto newHiddenWeights = transformer.TransformPortElements(_hiddenWeights.GetPortElements());
        auto newUpdateBias = transformer.TransformPortElements(_updateBias.GetPortElements());
        auto newResetBias = transformer.TransformPortElements(_resetBias.GetPortElements());
        auto newHiddenBias = transformer.TransformPortElements(_hiddenBias.GetPortElements());
        auto newNode = transformer.AddNode<GRUNode>(newInput, newUpdateWeights, newResetWeights, newHiddenWeights, newUpdateBias, newResetBias, newHiddenBias, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "GRUNode does not currently compute");
    }

    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* data, size_t dataLength)
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
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength)
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
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto minusFloat = emitters::TypedOperator::subtractFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;

        const size_t inputSize = this->input.Size();
        const size_t hiddenSize = this->updateBias.Size();

        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);
        RecurrentActivationFunctionType<ValueType> recurrentLayerActivationFunction;
        auto recurrentActivationFunction = GetNodeActivationFunction(recurrentLayerActivationFunction);

        // Get LLVM references for all node inputs
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* updateWeights = compiler.EnsurePortEmitted(this->updateWeights);
        llvm::Value* resetWeights = compiler.EnsurePortEmitted(this->resetWeights);
        llvm::Value* hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        llvm::Value* updateBias = compiler.EnsurePortEmitted(this->updateBias);
        llvm::Value* resetBias = compiler.EnsurePortEmitted(this->resetBias);
        llvm::Value* hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        // The node's output is the same as the hidden state --- just make an alias so the code looks nicer
        auto& hiddenState = pOutput;

        // Allocate local variables
        llvm::AllocaInst* inputPlusHidden = function.Variable(emitters::GetVariableType<ValueType>(), inputSize + hiddenSize);
        llvm::AllocaInst* z = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);
        llvm::AllocaInst* r = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);
        llvm::AllocaInst* hNew = function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize);

        // Concatenate input and hidden state into combined [Xt, Ht-1]
        function.MemoryCopy<ValueType>(pInput, inputPlusHidden, inputSize);
        function.MemoryCopy<ValueType>(hiddenState, 0, inputPlusHidden, inputSize, hiddenSize);

        // z = recurrentFunction(Wu * [Xt, Ht-1] + b_u)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(updateBias, z, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), updateWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), z, 1);
        ApplyActivation(function, recurrentActivationFunction, z, hiddenSize);

        // r = recurrentFunction(Wr * [Xt, Ht-1] + b_r)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(resetBias, r, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), resetWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), r, 1);
        ApplyActivation(function, recurrentActivationFunction, r, hiddenSize);

        // inputPlusHidden' = [Xt, Ht-1] .* r
        // in-place modify inputPlusHidden by scaling hidden part by r
        auto hiddenPart = function.PointerOffset(inputPlusHidden, inputSize);
        auto scaleByRLoop = function.ForLoop();
        scaleByRLoop.Begin(hiddenSize);
        {
            auto index = scaleByRLoop.LoadIterationVariable();
            auto scaledValue = function.Operator(timesFloat, function.ValueAt(r, index), function.ValueAt(hiddenPart, index));
            function.SetValueAt(hiddenPart, index, scaledValue);
        }
        scaleByRLoop.End();

        // hNew = activationFunction(Wh * inputPlusHidden' + b_h)  (where activationFunction is usually tanh)
        function.MemoryCopy<ValueType>(hiddenBias, hNew, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), hiddenWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), hNew, 1);
        ApplyActivation(function, activationFunction, hNew, hiddenSize);

        // hiddenState = (1-z).*hNew + z.*h
        auto newStateLoop = function.ForLoop();
        newStateLoop.Begin(hiddenSize);
        {
            auto index = newStateLoop.LoadIterationVariable();
            auto zVal = function.ValueAt(z, index);
            auto oneMinusZ = function.Operator(minusFloat, function.Literal<ValueType>(1.0), zVal);
            auto scaledOldH = function.Operator(timesFloat, oneMinusZ, function.ValueAt(hNew, index));
            auto scaledNewH = function.Operator(timesFloat, zVal, function.ValueAt(hiddenState, index));
            auto result = function.Operator(plusFloat, scaledOldH, scaledNewH);
            function.SetValueAt(hiddenState, index, result);
        }
        newStateLoop.End();

        // output <- hiddenState (no-op, since output and hidden state are aliases)
    }

    // most common: tanh, sigmoid
    // Explicit specialization
    template class GRULayerNode<float, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>;
    template class GRULayerNode<double, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>;
    template class GRUNode<float, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>;
    template class GRUNode<double, predictors::neural::TanhActivation, predictors::neural::SigmoidActivation>;
} // nodes
} // ell
