////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRULayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BroadcastFunctionNode.h"
#include "CompilableNodeUtilities.h" // for PortTypeToVariableType
#include "CompiledActivationFunctions.h"
#include "ConstantNode.h"
#include "GRULayerNode.h"
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
    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayerNode()
        : _reset(this, {}, "reset")
    {
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayerNode(const model::PortElements<ValueType>& input, const model::PortElements<int>& reset, const LayerType& layer)
        : BaseType(input, layer), _reset(this, reset, "reset")
    {
        this->AddInputPort(&_reset);
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

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    bool GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());
        ell::model::PortElements<int> newReset;
        if (this->reset.Size() == 0)
        {
            // this was a GRULayerNode deserialized from a model that didn't have the reset member.
            // So we provide a default here.
            auto defaultTriggerNode = transformer.AddNode<ConstantNode<int>>(0);
            newReset = defaultTriggerNode->output;
        }
        else
        {
            newReset = transformer.TransformPortElements(this->reset.GetPortElements());
        }

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
                                                                                     newReset,
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

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        ell::model::PortElements<int> newResetElements;
        if (this->reset.Size() == 0)
        {
            // this was a GRULayerNode deserialized from a model that didn't have the reset member.
            // So we provide a default here.
            auto defaultTriggerNode = transformer.AddNode<ConstantNode<int>>(0);
            newResetElements = defaultTriggerNode->output;
        }
        else
        {
            newResetElements = transformer.TransformPortElements(this->reset.GetPortElements());
        }
        auto newNode = transformer.AddNode<GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>>(newPortElements, newResetElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BaseType::WriteToArchive(archiver);
        archiver["reset"] << _reset;
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BaseType::ReadFromArchive(archiver);
        archiver["reset"] >> _reset;
        this->_output.SetSize(this->_layer.GetOutput().Size());
        this->AddInputPort(&_reset);
    }

    //
    // GRUNode
    //
    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRUNode()
        : GRUNode({ /*input*/ }, { /*resetTrigger*/ }, { /*updateWeights*/ }, { /*resetWeights*/ }, { /*hiddenWeights*/ }, { /*updateBias*/ }, { /*resetBias*/ }, { /*hiddenBias*/ }, { /*inputMemoryLayout*/ }, { /*outputMemoryLayout*/ })
    {
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::GRUNode(const model::PortElements<ValueType>& input,
                                                                                         const model::PortElements<int>& resetTrigger,
                                                                                         const model::PortElements<ValueType>& updateWeights,
                                                                                         const model::PortElements<ValueType>& resetWeights,
                                                                                         const model::PortElements<ValueType>& hiddenWeights,
                                                                                         const model::PortElements<ValueType>& updateBias,
                                                                                         const model::PortElements<ValueType>& resetBias,
                                                                                         const model::PortElements<ValueType>& hiddenBias,
                                                                                         const model::PortMemoryLayout& inputMemoryLayout,
                                                                                         const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode(std::vector<model::InputPortBase*>({ &_input, &_resetTrigger, &_updateWeights, &_resetWeights, &_hiddenWeights, &_updateBias, &_resetBias, &_hiddenBias }),
                         { &_output })
        , _input(this, input, defaultInputPortName)
        , _resetTrigger(this, resetTrigger, resetTriggerPortName)
        , _updateWeights(this, updateWeights, updateWeightsPortName)
        , _resetWeights(this, resetWeights, resetWeightsPortName)
        , _hiddenWeights(this, hiddenWeights, hiddenWeightsPortName)
        , _updateBias(this, updateBias, updateBiasPortName)
        , _resetBias(this, resetBias, resetBiasPortName)
        , _hiddenBias(this, hiddenBias, hiddenBiasPortName)
        , _output(this, defaultOutputPortName, outputMemoryLayout)
        , _inputMemoryLayout(inputMemoryLayout)
    {
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newResetTrigger = transformer.TransformPortElements(_resetTrigger.GetPortElements());
        auto newUpdateWeights = transformer.TransformPortElements(_updateWeights.GetPortElements());
        auto newResetWeights = transformer.TransformPortElements(_resetWeights.GetPortElements());
        auto newHiddenWeights = transformer.TransformPortElements(_hiddenWeights.GetPortElements());
        auto newUpdateBias = transformer.TransformPortElements(_updateBias.GetPortElements());
        auto newResetBias = transformer.TransformPortElements(_resetBias.GetPortElements());
        auto newHiddenBias = transformer.TransformPortElements(_hiddenBias.GetPortElements());
        auto newNode = transformer.AddNode<GRUNode>(newInput, newResetTrigger, newUpdateWeights, newResetWeights, newHiddenWeights, newUpdateBias, newResetBias, newHiddenBias, _inputMemoryLayout, GetOutputMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "GRUNode does not currently compute");
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Reset()
    {
        // noop until Compute() is implemented...
    }

    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    template<typename ActivationType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength)
    {
        function.For(dataLength, [activationFunction, data](emitters::IRFunctionEmitter& function, emitters::IRLocalScalar index) {
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
    template<typename ValueType, template<typename> class ActivationFunctionType, template<typename> class RecurrentActivationFunctionType>
    void GRUNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const size_t inputSize = this->input.Size();
        const size_t outputSize = this->updateBias.Size();

        ActivationFunctionType<ValueType> layerActivationFunction;
        auto activationFunction = GetNodeActivationFunction(layerActivationFunction);
        RecurrentActivationFunctionType<ValueType> recurrentLayerActivationFunction;
        auto recurrentActivationFunction = GetNodeActivationFunction(recurrentLayerActivationFunction);

        // Get LLVM references for all node inputs
        auto input = compiler.EnsurePortEmitted(this->input);
        auto resetTrigger = compiler.EnsurePortEmitted(this->resetTrigger);
        auto updateWeights = compiler.EnsurePortEmitted(this->updateWeights);
        auto resetWeights = compiler.EnsurePortEmitted(this->resetWeights);
        auto hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        auto updateBias = compiler.EnsurePortEmitted(this->updateBias);
        auto resetBias = compiler.EnsurePortEmitted(this->resetBias);
        auto hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output));

        // Allocate global buffer for hidden state
        emitters::IRModuleEmitter& module = function.GetModule();
        emitters::VariableType varType = PortTypeToVariableType(this->output.GetType());
        auto hiddenStateVariable = module.Variables().AddVectorVariable(emitters::VariableScope::global, varType, outputSize);
        auto hiddenStateValue = module.EnsureEmitted(*hiddenStateVariable);
        auto hiddenState = function.LocalArray(hiddenStateValue);
        auto& prevHiddenState = hiddenState;

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
        function.For(outputSize, [=](emitters::IRFunctionEmitter& function, emitters::IRLocalScalar index) {
            hiddenPart[index] = resetGateActivation[index] * hiddenPart[index];
        });

        // Now, inputPlusHidden == Rt . * [Xt, Ht-1]
        // newHiddenState = Ht~ = activationFunction(Wh * inputPlusHidden + b_h)  (where activationFunction is usually tanh)
        function.MemoryCopy<ValueType>(hiddenBias, newHiddenState, outputSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(outputSize, inputSize + outputSize, static_cast<ValueType>(1.0), hiddenWeights, inputSize + outputSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), newHiddenState, 1);
        ApplyActivation(function, activationFunction, newHiddenState, outputSize);

        // Compute Ht = (1-Zt) .* Ht~ + Zt * Ht-1,
        function.For(outputSize, [=](emitters::IRFunctionEmitter& function, emitters::IRLocalScalar index) {
            auto z_i = updateGateActivation[index];

            // Note: Keep the static cast here -- using 1.0 directly results in NaN
            auto newValue = ((static_cast<ValueType>(1.0) - z_i) * newHiddenState[index]) + (z_i * prevHiddenState[index]);
            output[index] = newValue;
        });

        // save new hidden state
        function.MemoryCopy<ValueType>(output, 0, hiddenState, 0, outputSize);

        // Add the internal reset function
        std::string resetFunctionName = compiler.GetGlobalName(*this, "GRUNodeReset");
        emitters::IRFunctionEmitter& resetFunction = module.BeginResetFunction(resetFunctionName);
        auto resetHiddenState = resetFunction.LocalArray(hiddenStateValue);
        resetFunction.MemorySet<ValueType>(resetHiddenState, 0, function.Literal<uint8_t>(0), outputSize);
        // resetFunction.Print("### GRU Node was reset\n"); // this is a handy way to debug whether the VAD node is working or not.
        module.EndResetFunction();

        // if the reset trigger drops to zero then it means it is time to reset this node, but only do this when signal transitions from 1 to 0
        // Allocate global variable to hold the previous trigger value so we can detect the change in state.
        auto lastSignal = module.Global<int>(compiler.GetGlobalName(*this, "lastSignal"), 0);
        auto lastSignalValue = function.LocalScalar(function.Load(lastSignal));
        auto resetTriggerValue = function.LocalScalar(resetTrigger);
        function.If((resetTriggerValue == 0) && (lastSignalValue == 1), [resetFunctionName](emitters::IRFunctionEmitter& fn) {
            fn.Call(resetFunctionName);
        });
        function.Store(lastSignal, resetTriggerValue);
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
