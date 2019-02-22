////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RNNNode.cpp (nodes)
//  Authors:  James Devine, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RNNNode.h"
#include "ActivationFunctions.h"

#include <emitters/include/IRMath.h>

#include <math/include/MatrixOperations.h>

#include <utilities/include/Exception.h>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    RNNNode<ValueType>::RNNNode() :
        CompilableNode(
            { &_input, &_resetTrigger, &_inputWeights, &_hiddenWeights, &_inputBias, &_hiddenBias },
            { &_output }),
        _input(this, {}, defaultInputPortName),
        _resetTrigger(this, resetTriggerPortName),
        _hiddenUnits(0),
        _inputWeights(this, {}, inputWeightsPortName),
        _hiddenWeights(this, {}, hiddenWeightsPortName),
        _inputBias(this, {}, inputBiasPortName),
        _hiddenBias(this, {}, hiddenBiasPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    RNNNode<ValueType>::RNNNode(const model::OutputPort<ValueType>& input,
                                const model::OutputPortBase& resetTrigger,
                                size_t hiddenUnits,
                                const model::OutputPort<ValueType>& inputWeights,
                                const model::OutputPort<ValueType>& hiddenWeights,
                                const model::OutputPort<ValueType>& inputBias,
                                const model::OutputPort<ValueType>& hiddenBias,
                                const ActivationType& activation,
                                bool validateWeights) :
        CompilableNode({ &_input, &_resetTrigger, &_inputWeights, &_hiddenWeights, &_inputBias, &_hiddenBias },
                       { &_output }),
        _input(this, input, defaultInputPortName),
        _resetTrigger(this, resetTrigger, resetTriggerPortName),
        _hiddenUnits(hiddenUnits),
        _inputWeights(this, inputWeights, inputWeightsPortName),
        _hiddenWeights(this, hiddenWeights, hiddenWeightsPortName),
        _inputBias(this, inputBias, inputBiasPortName),
        _hiddenBias(this, hiddenBias, hiddenBiasPortName),
        _output(this, defaultOutputPortName, hiddenUnits),
        _activation(activation),
        _hiddenState(hiddenUnits)
    {
        if (validateWeights)
        {
            size_t numRows = hiddenUnits;
            size_t numColumns = input.Size();

            if (inputWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The RNNNode input weights are the wrong size, found %zu but expecting %zu", inputWeights.Size(), numRows * numColumns));
            }

            numColumns = hiddenUnits;
            if (hiddenWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The RNNNode hidden weights are the wrong size, found %zu but expecting %zu", hiddenWeights.Size(), numRows * numColumns));
            }

            if (inputBias.Size() != hiddenUnits)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The RNNNode input bias vector is the wrong size, found %zu but expecting %zu", inputBias.Size(), hiddenUnits));
            }

            if (hiddenBias.Size() != hiddenUnits)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The RNNNode hidden bias vector is the wrong size, found %zu but expecting %zu", hiddenBias.Size(), hiddenUnits));
            }
        }
    }

    template <typename ValueType>
    void RNNNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->_input);
        const auto& newResetTrigger = transformer.GetCorrespondingInputs(this->_resetTrigger);
        const auto& newInputWeights = transformer.GetCorrespondingInputs(this->_inputWeights);
        const auto& newHiddenWeights = transformer.GetCorrespondingInputs(this->_hiddenWeights);
        const auto& newInputBias = transformer.GetCorrespondingInputs(this->_inputBias);
        const auto& newHiddenBias = transformer.GetCorrespondingInputs(this->_hiddenBias);
        auto newNode = transformer.AddNode<RNNNode>(newInput, newResetTrigger, this->_hiddenUnits, newInputWeights, newHiddenWeights, newInputBias, newHiddenBias, this->_activation);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void RNNNode<ValueType>::Compute() const
    {
        using ConstMatrixReferenceType = math::ConstRowMatrixReference<ValueType>;

        // it = sigma(W_{ ii } x + b_{ ii } +W_{ hi } h + b_{ hi })
        // h = tanh(it)

        size_t hiddenUnits = this->_hiddenUnits;
        VectorType inputVector(this->_input.GetValue());
        size_t numRows = hiddenUnits;
        size_t numColumns = inputVector.Size();
        std::vector<ValueType> inputWeightsValue = this->_inputWeights.GetValue();
        ConstMatrixReferenceType inputWeights(inputWeightsValue.data(), numRows, numColumns);
        numColumns = hiddenUnits;
        std::vector<ValueType> hiddenWeightsValue = this->_hiddenWeights.GetValue();
        ConstMatrixReferenceType hiddenWeights(hiddenWeightsValue.data(), numRows, numColumns);
        VectorType inputBias(this->_inputBias.GetValue());
        VectorType hiddenBias(this->_hiddenBias.GetValue());

        auto alpha = static_cast<ValueType>(1); // GEMV scale multiplication
        auto beta = static_cast<ValueType>(1); // GEMV scale bias

        // W_i * x + b_i
        VectorType input_gate(inputBias); // add input bias
        math::MultiplyScaleAddUpdate(alpha, inputWeights, inputVector, beta, input_gate);

        // Wh * h + b_h
        VectorType hidden_gate(hiddenBias); // add hidden bias
        math::MultiplyScaleAddUpdate(alpha, hiddenWeights, this->_hiddenState, beta, hidden_gate);

        // compute: W_{ ii } x + b_{ ii } +W_{ hi } h + b_{ hi }
        input_gate += hidden_gate;

        // tanh(...)
        this->_activation.Apply(input_gate);

        // save new state.
        this->_hiddenState.CopyFrom(input_gate);

        if (ShouldReset())
        {
            const_cast<RNNNode<ValueType>*>(this)->Reset();
        }

        // copy to output.
        this->_output.SetOutput(this->_hiddenState.ToArray());
    }

    template <typename ValueType>
    bool RNNNode<ValueType>::ShouldReset() const
    {
        // Helper function for Compute to use to figure out if node should be reset using the _resetTrigger input.
        bool result = false;
        
        if (this->_resetTrigger.Size() > 0)
        {
            auto triggerValue = this->_resetTrigger.GetInputElement(0).ReferencedPort()->GetDoubleOutput(0);
            int vad = static_cast<int>(triggerValue);
            if (this->_lastResetValue != 0 && vad == 0)
            {
                result = true;
            }
            this->_lastResetValue = vad;
        }
        return result;
    }

    template <typename ValueType>
    void RNNNode<ValueType>::Reset()
    {
        this->_hiddenState.Reset();
    }

    template <typename ValueType>
    void RNNNode<ValueType>::ApplyActivation(emitters::IRFunctionEmitter& function, const ActivationType& activation, emitters::LLVMValue data, size_t dataLength)
    {
        auto activationFunction = GetNodeActivationFunction(activation);
        function.For(dataLength, [&](emitters::IRFunctionEmitter& function, auto i) {
            emitters::LLVMValue inputValue = function.ValueAt(data, i);
            emitters::LLVMValue x = activationFunction->Compile(function, inputValue);
            function.SetValueAt(data, i, x);
        });
    }

    template <typename ValueType>
    void RNNNode<ValueType>::ApplySoftmax(emitters::IRFunctionEmitter& function, emitters::LLVMValue dataValue, size_t dataLength)
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
    void RNNNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // it = sigma(W_{ ii } x + b_{ ii } +W_{ hi } h + b_{ hi })
        // h = tanh(it)
        const int hiddenUnits = static_cast<int>(this->_hiddenUnits);
        const int inputSize = static_cast<int>(this->input.Size());

        // Get LLVM references for all node inputs
        auto input = compiler.EnsurePortEmitted(this->input);
        auto resetTrigger = compiler.EnsurePortEmitted(this->resetTrigger);
        auto inputWeights = compiler.EnsurePortEmitted(this->inputWeights);
        auto hiddenWeights = compiler.EnsurePortEmitted(this->hiddenWeights);
        auto inputBias = compiler.EnsurePortEmitted(this->inputBias);
        auto hiddenBias = compiler.EnsurePortEmitted(this->hiddenBias);

        // Get LLVM reference for node output
        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output));

        // Allocate global buffer for hidden state
        emitters::IRModuleEmitter& module = function.GetModule();
        auto hiddenStateVariable = module.Variables().AddVectorVariable<ValueType>(emitters::VariableScope::global, hiddenUnits);
        auto hiddenStateValue = module.EnsureEmitted(*hiddenStateVariable);
        auto hiddenStatePointer = function.PointerOffset(hiddenStateValue, 0); // convert "global variable" to a pointer
        auto hiddenState = function.LocalArray(hiddenStatePointer);
        auto& prevHiddenState = hiddenState;

        // Allocate local variables
        auto inputGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto hiddenGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));

        auto alpha = static_cast<ValueType>(1.0); // GEMV scaling of the matrix multipication
        auto beta = static_cast<ValueType>(1.0); // GEMV scaling of the bias addition

        // W_i * x + b_i
        function.MemoryCopy<ValueType>(inputBias, inputGate, hiddenUnits); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenUnits, inputSize, alpha, inputWeights, inputSize, input, 1, beta, inputGate, 1);

        // W_h * h + b_h
        function.MemoryCopy<ValueType>(hiddenBias, hiddenGate, hiddenUnits); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(hiddenUnits, hiddenUnits, alpha, hiddenWeights, hiddenUnits, prevHiddenState, 1, beta, hiddenGate, 1);

        // input_gate = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            inputGate[i] = inputGate[i] + hiddenGate[i];
        });

        // tanh
        this->ApplyActivation(function, this->_activation, inputGate, hiddenUnits);

        // save new HiddenState
        function.MemoryCopy<ValueType>(inputGate, hiddenState, hiddenUnits);

        // Copy hidden state to the output.
        function.MemoryCopy<ValueType>(hiddenState, output, hiddenUnits);

        // Add the internal reset function
        std::string resetFunctionName = compiler.GetGlobalName(*this, "RNNNodeReset");
        emitters::IRFunctionEmitter& resetFunction = module.BeginResetFunction(resetFunctionName);
        auto resetHiddenState = resetFunction.LocalArray(hiddenStateValue);
        resetFunction.MemorySet<ValueType>(resetHiddenState, 0, function.Literal<uint8_t>(0), hiddenUnits);
        // resetFunction.Print("### LSTM Node was reset\n"); // this is a handy way to debug whether the VAD node is working or not.
        module.EndResetFunction();

        // if the reset trigger drops to zero then it means it is time to reset this node, but only do this when signal transitions from 1 to 0
        // Allocate global variable to hold the previous trigger value so we can detect the change in state.
        auto lastSignal = module.Global<int>(compiler.GetGlobalName(*this, "lastSignal"), 0);
        auto lastSignalValue = function.LocalScalar(function.Load(lastSignal));
        auto resetTriggerValue = function.LocalScalar(function.CastValue<int>(function.Load(resetTrigger)));
        function.If((resetTriggerValue == 0) && (lastSignalValue != 0), [resetFunctionName](emitters::IRFunctionEmitter& fn) {
            fn.Call(resetFunctionName);
        });
        function.Store(lastSignal, resetTriggerValue);
    }

    template <typename ValueType>
    void RNNNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[resetTriggerPortName] << _resetTrigger;
        archiver["hiddenUnits"] << _hiddenUnits;
        archiver[inputWeightsPortName] << _inputWeights;
        archiver[hiddenWeightsPortName] << _hiddenWeights;
        archiver[inputBiasPortName] << _inputBias;
        archiver[hiddenBiasPortName] << _hiddenBias;

        _activation.WriteToArchive(archiver);
    }

    template <typename ValueType>
    void RNNNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[resetTriggerPortName] >> _resetTrigger;
        archiver["hiddenUnits"] >> _hiddenUnits;
        archiver[inputWeightsPortName] >> _inputWeights;
        archiver[hiddenWeightsPortName] >> _hiddenWeights;
        archiver[inputBiasPortName] >> _inputBias;
        archiver[hiddenBiasPortName] >> _hiddenBias;

        _activation.ReadFromArchive(archiver);

        _hiddenState.Resize(_hiddenUnits);
        this->_output.SetSize(_hiddenUnits);
    }

    // Explicit instantiations
    template class RNNNode<float>;
    template class RNNNode<double>;
} // namespace nodes
} // namespace ell
