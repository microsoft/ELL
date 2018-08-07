////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMLayerNode.cpp (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BroadcastFunctionNode.h"
#include "CompiledActivationFunctions.h" // For sigmoid and tanh
#include "ConstantNode.h"
#include "LSTMLayerNode.h"
#include "MatrixVectorMultiplyNode.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template<typename ValueType>
    LSTMLayerNode<ValueType>::LSTMLayerNode()
        : _reset(this, {}, "reset")
    {
    }

    template<typename ValueType>
    LSTMLayerNode<ValueType>::LSTMLayerNode(const model::PortElements<ValueType>& input, const model::PortElements<int>& reset, const LayerType& layer)
        : BaseType(input, layer), _reset(this, reset, "reset")
    {
        this->AddInputPort(&_reset);
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

    template<typename ValueType>
    bool LSTMLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());
        ell::model::PortElements<int> newReset;
        if (this->reset.Size() == 0)
        {
            // this was a LSTMLayerNode deserialized from a model that didn't have the reset member.
            // So we provide a default here.
            auto defaultTriggerNode = transformer.AddNode<ConstantNode<int>>(0);
            newReset = defaultTriggerNode->output;
        }
        else
        {
            newReset = transformer.TransformPortElements(this->reset.GetPortElements());
        }

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

        using ComputeNodeType = LSTMNode<ValueType>;
        auto lstmNode = transformer.AddNode<ComputeNodeType>(newInput,
                                                             newReset,
                                                             inputWeightsNode->output,
                                                             forgetMeWeightsNode->output,
                                                             candidateWeightsNode->output,
                                                             outputWeightsNode->output,
                                                             inputBiasNode->output,
                                                             forgetMeBiasNode->output,
                                                             candidateBiasNode->output,
                                                             outputBiasNode->output,
                                                             this->_layer.GetActivationFunction(),
                                                             this->_layer.GetRecurrentActivationFunction(),
                                                             this->GetInputMemoryLayout(),
                                                             this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, lstmNode->output);
        return true;
    }

    template<typename ValueType>
    void LSTMLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        ell::model::PortElements<int> newResetElements;
        if (this->reset.Size() == 0)
        {
            // this was a LSTMLayerNode deserialized from a model that didn't have the reset member.
            // So we provide a default here.
            auto defaultTriggerNode = transformer.AddNode<ConstantNode<int>>(0);
            newResetElements = defaultTriggerNode->output;
        }
        else
        {
            newResetElements = transformer.TransformPortElements(this->reset.GetPortElements());
        }
        auto newNode = transformer.AddNode<LSTMLayerNode<ValueType>>(newPortElements, newResetElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    template<typename ValueType>
    void LSTMLayerNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BaseType::WriteToArchive(archiver);
        archiver["reset"] << _reset;
    }

    template<typename ValueType>
    void LSTMLayerNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BaseType::ReadFromArchive(archiver);
        archiver["reset"] >> _reset;
        this->_output.SetSize(this->_layer.GetOutput().Size());
        this->AddInputPort(&_reset);
    }

    //
    // LSTMNode
    //
    template<typename ValueType>
    LSTMNode<ValueType>::LSTMNode()
        : CompilableNode({ &_input, &_resetTrigger, &_inputWeights, &_forgetMeWeights, &_candidateWeights, &_outputWeights, &_inputBias, &_forgetMeBias, &_candidateBias, &_outputBias },
                         { &_output })
        , _input(this, {}, defaultInputPortName)
        , _resetTrigger(this, {}, resetTriggerPortName)
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

    template<typename ValueType>
    LSTMNode<ValueType>::LSTMNode(const model::PortElements<ValueType>& input,
                                                                                           const model::PortElements<int>& resetTrigger,
                                                                                           const model::PortElements<ValueType>& inputWeights,
                                                                                           const model::PortElements<ValueType>& forgetMeWeights,
                                                                                           const model::PortElements<ValueType>& candidateWeights,
                                                                                           const model::PortElements<ValueType>& outputWeights,
                                                                                           const model::PortElements<ValueType>& inputBias,
                                                                                           const model::PortElements<ValueType>& forgetMeBias,
                                                                                           const model::PortElements<ValueType>& candidateBias,
                                                                                           const model::PortElements<ValueType>& outputBias,
                                                                                           const ActivationType& activation,
                                                                                           const ActivationType& recurrentActivation,
                                                                                           const model::PortMemoryLayout& inputMemoryLayout,
                                                                                           const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_resetTrigger, &_inputWeights, &_forgetMeWeights, &_candidateWeights, &_outputWeights, &_inputBias, &_forgetMeBias, &_candidateBias, &_outputBias },
                         { &_output })
        , _input(this, input, defaultInputPortName)
        , _resetTrigger(this, resetTrigger, resetTriggerPortName)
        , _inputWeights(this, inputWeights, inputWeightsPortName)
        , _forgetMeWeights(this, forgetMeWeights, forgetMeWeightsPortName)
        , _candidateWeights(this, candidateWeights, candidateWeightsPortName)
        , _outputWeights(this, outputWeights, outputWeightsPortName)
        , _inputBias(this, inputBias, inputBiasPortName)
        , _forgetMeBias(this, forgetMeBias, forgetMeBiasPortName)
        , _candidateBias(this, candidateBias, candidateBiasPortName)
        , _outputBias(this, outputBias, outputBiasPortName)
        , _output(this, defaultOutputPortName, inputBias.Size())
        , _activation(activation)
        , _recurrentActivation(recurrentActivation)
    {
    }

    template<typename ValueType>
    void LSTMNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newResetTrigger = transformer.TransformPortElements(_resetTrigger.GetPortElements());
        auto newInputWeights = transformer.TransformPortElements(_inputWeights.GetPortElements());
        auto newForgetMeWeights = transformer.TransformPortElements(_forgetMeWeights.GetPortElements());
        auto newCandidateWeights = transformer.TransformPortElements(_candidateWeights.GetPortElements());
        auto newOutputWeights = transformer.TransformPortElements(_outputWeights.GetPortElements());
        auto newInputBias = transformer.TransformPortElements(_inputBias.GetPortElements());
        auto newForgetMeBias = transformer.TransformPortElements(_forgetMeBias.GetPortElements());
        auto newCandidateBias = transformer.TransformPortElements(_candidateBias.GetPortElements());
        auto newOutputBias = transformer.TransformPortElements(_outputBias.GetPortElements());
        auto newNode = transformer.AddNode<LSTMNode>(newInput, newResetTrigger, newInputWeights, newForgetMeWeights, newCandidateWeights, newOutputWeights, newInputBias, newForgetMeBias, newCandidateBias, newOutputBias, _activation, _recurrentActivation, _inputMemoryLayout, GetOutputMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template<typename ValueType>
    void LSTMNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "LSTMNode does not currently compute");
    }

    template<typename ValueType>
    void LSTMNode<ValueType>::Reset()
    {
        // noop until Compute() is implemented...
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* dataValue, size_t dataLength)
    {
        auto data = function.LocalArray(dataValue);
        auto sum = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), 1));
        sum[0] = function.Literal<ValueType>(0);

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
    void LSTMNode<ValueType>::ApplyActivation(emitters::IRFunctionEmitter& function, const ActivationType& activation, llvm::Value* data, size_t dataLength)
    {
        auto activationFunction = GetNodeActivationFunction<ValueType>(activation);
        function.For(dataLength, [&](emitters::IRFunctionEmitter& function, auto i) {
            auto inputValue = function.ValueAt(data, i);
            auto x = activationFunction->Compile(function, inputValue);
            function.SetValueAt(data, i, x);
        });
    }

    template<typename ValueType>
    void LSTMNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const size_t inputSize = this->input.Size();
        const size_t hiddenSize = this->inputBias.Size();

        size_t outputSize = this->output.Size();
        if (outputSize > hiddenSize)
        {
            outputSize = hiddenSize;
        }
        
        // Global state (in addition to output)
        emitters::IRModuleEmitter& module = function.GetModule();
        emitters::VariableType varType = emitters::GetVariableType<ValueType>();
        auto ctActual = module.Variables().AddVectorVariable(emitters::VariableScope::global, varType, hiddenSize);
        auto ctActualValue = function.LocalArray(module.EnsureEmitted(*ctActual));

        // Get LLVM references for all node inputs
        auto input = compiler.EnsurePortEmitted(this->input);
        auto resetTrigger = compiler.EnsurePortEmitted(this->resetTrigger);
        auto inputWeights = compiler.EnsurePortEmitted(this->inputWeights);
        auto forgetMeWeights = compiler.EnsurePortEmitted(this->forgetMeWeights);
        auto candidateWeights = compiler.EnsurePortEmitted(this->candidateWeights);
        auto outputWeights = compiler.EnsurePortEmitted(this->outputWeights);
        auto inputBias = compiler.EnsurePortEmitted(this->inputBias);
        auto forgetMeBias = compiler.EnsurePortEmitted(this->forgetMeBias);
        auto candidateBias = compiler.EnsurePortEmitted(this->candidateBias);
        auto outputBias = compiler.EnsurePortEmitted(this->outputBias);

        // Get LLVM reference for node output
        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

        // Allocate global buffer for hidden state
        auto hiddenStateVariable = module.Variables().AddVectorVariable(emitters::VariableScope::global, varType, hiddenSize);
        auto hiddenStateValue = module.EnsureEmitted(*hiddenStateVariable);
        auto hiddenState = function.LocalArray(hiddenStateValue);

        // Allocate local variables
        auto inputPlusHidden = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), inputSize + hiddenSize));
        auto ft = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize));
        auto it = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize));
        auto ctNew = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize));
        auto ot = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenSize));

        // Concatenate input and hidden state into combined [Xt, Ht-1]
        function.MemoryCopy<ValueType>(input, inputPlusHidden, inputSize);
        function.MemoryCopy<ValueType>(hiddenState, 0, inputPlusHidden, inputSize, hiddenSize);

        // ft = recurrentFunction(Wf * [Xt, Ht-1] + Bf)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(forgetMeBias, ft, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), forgetMeWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ft, 1);
        ApplyActivation(function, _recurrentActivation, ft, hiddenSize);

        // it = recurrentFunction(Wi * [Xt, Ht-1] + Bi)    (where recurrentFunction is usually sigmoid)
        function.MemoryCopy<ValueType>(inputBias, it, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), inputWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), it, 1);
        ApplyActivation(function, _recurrentActivation, it, hiddenSize);

        // Ct~ = activationFunction(Wc * [Xt, Ht-1] + Bc)  (where activationFunction is usually tanh)
        function.MemoryCopy<ValueType>(candidateBias, ctNew, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), candidateWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ctNew, 1);
        ApplyActivation(function, _activation, ctNew, hiddenSize);

        // Ct = ft * Ct-1 + it * Ct~
        function.For(hiddenSize, [ctActualValue, ft, it, ctNew](emitters::IRFunctionEmitter& function, llvm::Value* i) {
            auto index = function.LocalScalar(i);
            auto ctActualVal = ctActualValue[index];
            auto ftVal = ft[index];
            auto itVal = it[index];
            auto ctNewVal = ctNew[index];

            auto ftCt = ftVal * ctActualVal;
            auto itctNew = itVal * ctNewVal;

            auto result = ftCt + itctNew;
            ctActualValue[index] = result;
        });

        // ot = recurrentFunction(Wo * [Xt, Ht-1] + Bo)
        function.MemoryCopy<ValueType>(outputBias, ot, hiddenSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenSize, inputSize + hiddenSize, static_cast<ValueType>(1.0), outputWeights, inputSize + hiddenSize, inputPlusHidden, 1, static_cast<ValueType>(1.0), ot, 1);
        ApplyActivation(function, _recurrentActivation, ot, hiddenSize);

        // compute the hidden layer and copy into state vector
        // ht = ot * activationFunction(Ct)  (where activationFunction is usually tanh)
        ApplyActivation(function, _activation, ctActualValue, hiddenSize);

        function.For(hiddenSize, [ot, ctActualValue, hiddenState](emitters::IRFunctionEmitter& function, auto index) {
            auto otVal = ot[index];
            auto ctVal = ctActualValue[index];
            auto result = ctVal * otVal;
            hiddenState[index] = result;
        });

        // output <- hiddenState
        function.MemoryCopy<ValueType>(hiddenState, 0, output, 0, outputSize);

        // Add the internal reset function
        std::string resetFunctionName = compiler.GetGlobalName(*this, "LSTMNodeReset");
        emitters::IRFunctionEmitter& resetFunction = module.BeginResetFunction(resetFunctionName);
        auto resetctState = resetFunction.LocalArray(ctActualValue);
        resetFunction.MemorySet<ValueType>(resetctState, 0, function.Literal<uint8_t>(0), hiddenSize);
        auto resetHiddenState = resetFunction.LocalArray(ctActualValue);
        resetFunction.MemorySet<ValueType>(resetHiddenState, 0, function.Literal<uint8_t>(0), hiddenSize);
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
template class LSTMLayerNode<float>;  
template class LSTMLayerNode<double>; 
template class LSTMNode<float>;       
template class LSTMNode<double>;

} // nodes
} // ell
