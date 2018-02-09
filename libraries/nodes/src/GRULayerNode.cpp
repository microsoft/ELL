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
    GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
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
    template <typename ActivationType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength)
    {
        function.For(dataLength, [activationFunction, data](emitters::IRFunctionEmitter& function, llvm::Value* index) {
            auto dataArray = function.LocalArray(data);
            dataArray[index] = activationFunction.Compile(function, static_cast<emitters::IRLocalScalar>(dataArray[index]));
        });
    }

    // Notation:
    // The notation in the comments is adapted from the explanation at http://colah.github.io/posts/2015-08-Understanding-LSTMs/
    //
    // Wu == updateWeights
    // Wr == resetWeights
    // Wh == hiddenWeights (aka output weights)
    // Bu == updateBias
    // Br == resetBias
    // Bh == hiddenBias
    //
    // Zt == updateGateActivation
    // Rt == resetGateActivation
    // 
    // [Xt, Ht-1] == inputPlusHidden
    // Ht~ == newHiddenState
    // Ht == hiddenState (aka, output)
    //
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const size_t inputSize = this->input.Size();
        const size_t outputSize = this->updateBias.Size();

        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);
        RecurrentActivationFunctionType<ValueType> recurrentLayerActivationFunction;
        auto recurrentActivationFunction = GetNodeActivationFunction(recurrentLayerActivationFunction);

        // Get LLVM references for all node inputs
        llvm::Value* input = compiler.EnsurePortEmitted(this->input);
        llvm::Value* updateWeights = compiler.EnsurePortEmitted(this->updateWeights);
        llvm::Value* resetWeights = compiler.EnsurePortEmitted(this->resetWeights);
        llvm::Value* hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        llvm::Value* updateBias = compiler.EnsurePortEmitted(this->updateBias);
        llvm::Value* resetBias = compiler.EnsurePortEmitted(this->resetBias);
        llvm::Value* hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output));

        // The node's output is the same as the hidden state --- just make an alias so the code looks nicer
        // Same goes for prevHiddenState
        auto& hiddenState = output;
        auto& prevHiddenState = output;

        // Allocate local variables
        auto inputPlusHidden = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), inputSize + outputSize));
        auto updateGateActivation = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), outputSize));
        auto resetGateActivation = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), outputSize));
        auto newHiddenState = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), outputSize));

        // Concatenate input and hidden state into inputPlusHidden: [Xt, Ht-1]
        function.MemoryCopy<ValueType>(input, inputPlusHidden, inputSize);
        function.MemoryCopy<ValueType>(hiddenState, 0, inputPlusHidden, inputSize, outputSize);
        // Now, inputPlusHidden = [Xt, Ht-1]

        // Zt = recurrentFunction(Wu * [Xt, Ht-1] + Bu)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(updateBias, updateGateActivation, outputSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(outputSize, inputSize + outputSize, static_cast<ValueType>(1.0), updateWeights, inputSize + outputSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), updateGateActivation, 1);
        ApplyActivation(function, recurrentActivationFunction, updateGateActivation, outputSize);

        // Rt = recurrentFunction(Wr * [Xt, Ht-1] + Br)   (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(resetBias, resetGateActivation, outputSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(outputSize, inputSize + outputSize, static_cast<ValueType>(1.0), resetWeights, inputSize + outputSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), resetGateActivation, 1);
        ApplyActivation(function, recurrentActivationFunction, resetGateActivation, outputSize);

        // Ht~ = activationFunction(Wh * [Xt, (Rt .* Ht-1)] + Bh)   (where activationFunction is typically tanh)

        // in-place modify inputPlusHidden by scaling hidden part by resetGateActivation
        auto hiddenPart = function.LocalArray(function.PointerOffset(inputPlusHidden, inputSize));
        function.For(outputSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* index) {
            hiddenPart[index] = resetGateActivation[index] * hiddenPart[index];
        });

        // Now, inputPlusHidden == Rt . * [Xt, Ht-1]
        // newHiddenState = Ht~ = activationFunction(Wh * inputPlusHidden + b_h)  (where activationFunction is usually tanh)
        function.MemoryCopy<ValueType>(hiddenBias, newHiddenState, outputSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(outputSize, inputSize + outputSize, static_cast<ValueType>(1.0), hiddenWeights, inputSize + outputSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), newHiddenState, 1);
        ApplyActivation(function, activationFunction, newHiddenState, outputSize);

        // Compute Ht = (1-Zt) .* Ht~ + Zt * Ht-1,
        function.For(outputSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* index) {
            auto z_i = updateGateActivation[index];

            // Note: Keep the static cast here -- using 1.0 directly results in NaN
            auto newValue = ((static_cast<ValueType>(1.0) - z_i) * newHiddenState[index]) + (z_i * prevHiddenState[index]);
            output[index] = newValue;
        });
    }

// Explicit specialization
#define INSTANTIATE_GRU(activation1, activation2)                  \
    template class GRULayerNode<float, activation1, activation2>;  \
    template class GRULayerNode<double, activation1, activation2>; \
    template class GRUNode<float, activation1, activation2>;       \
    template class GRUNode<double, activation1, activation2>

    INSTANTIATE_GRU(predictors::neural::HardSigmoidActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::HardSigmoidActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_GRU(predictors::neural::HardSigmoidActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::HardSigmoidActivation, predictors::neural::TanhActivation);

    INSTANTIATE_GRU(predictors::neural::ReLUActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::ReLUActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_GRU(predictors::neural::ReLUActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::ReLUActivation, predictors::neural::TanhActivation);

    INSTANTIATE_GRU(predictors::neural::SigmoidActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::SigmoidActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_GRU(predictors::neural::SigmoidActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::SigmoidActivation, predictors::neural::TanhActivation);

    INSTANTIATE_GRU(predictors::neural::TanhActivation, predictors::neural::HardSigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::TanhActivation, predictors::neural::ReLUActivation);
    INSTANTIATE_GRU(predictors::neural::TanhActivation, predictors::neural::SigmoidActivation);
    INSTANTIATE_GRU(predictors::neural::TanhActivation, predictors::neural::TanhActivation);
} // nodes
} // ell
