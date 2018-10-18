////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMNode.cpp (nodes)
//  Authors:  James Devine, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LSTMNode.h"

#include "MatrixOperations.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    //
    // LSTMNode
    //
    template <typename ValueType>
    LSTMNode<ValueType>::LSTMNode()
        : RNNNode<ValueType>()
    {
    }

    template <typename ValueType>
    LSTMNode<ValueType>::LSTMNode(const model::OutputPort<ValueType>& input,
                                  const model::OutputPort<int>& resetTrigger,
                                  size_t hiddenUnits,
                                  const model::OutputPort<ValueType>& inputWeights,
                                  const model::OutputPort<ValueType>& hiddenWeights,
                                  const model::OutputPort<ValueType>& inputBias,
                                  const model::OutputPort<ValueType>& hiddenBias,
                                  const ActivationType& activation,
                                  const ActivationType& recurrentActivation,
                                  bool validateWeights)
        : RNNNode<ValueType>(input, resetTrigger, hiddenUnits, inputWeights, hiddenWeights, inputBias, hiddenBias, activation, false)
        , _recurrentActivation(recurrentActivation)
        , _cellState(hiddenUnits)
    {
        if (validateWeights)
        {
            size_t stackHeight = 4; // LSTM has 4 stacked weights for (input, forget, cell, output).
            size_t numRows = stackHeight * hiddenUnits;
            size_t numColumns = input.Size();
            if (inputWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    ell::utilities::FormatString("The LSTMNode input weights are the wrong size, found %zu but expecting %zu", inputWeights.Size(), numRows * numColumns));
            }

            numColumns = hiddenUnits;
            if (hiddenWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    ell::utilities::FormatString("The LSTMNode hidden weights are the wrong size, found %zu but expecting %zu", hiddenWeights.Size(), numRows * numColumns));
            }
            
            if (inputBias.Size() != numRows)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    ell::utilities::FormatString("The LSTMNode input bias vector is the wrong size, found %zu but expecting %zu", inputBias.Size(), numRows));
            }

            if (hiddenBias.Size() != numRows)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    ell::utilities::FormatString("The LSTMNode hidden bias vector is the wrong size, found %zu but expecting %zu", hiddenBias.Size(), numRows));
            }
        }
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->_input);
        const auto& newResetTrigger = transformer.GetCorrespondingInputs(this->_resetTrigger);
        const auto& newInputWeights = transformer.GetCorrespondingInputs(this->_inputWeights);
        const auto& newHiddenWeights = transformer.GetCorrespondingInputs(this->_hiddenWeights);
        const auto& newInputBias = transformer.GetCorrespondingInputs(this->_inputBias);
        const auto& newHiddenBias = transformer.GetCorrespondingInputs(this->_hiddenBias);
        auto newNode = transformer.AddNode<LSTMNode>(newInput, newResetTrigger, this->_hiddenUnits, newInputWeights, newHiddenWeights, newInputBias, newHiddenBias, this->_activation, this->_recurrentActivation);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::Compute() const
    {
        using ConstMatrixReferenceType = math::ConstRowMatrixReference<ValueType>;

        /*
        it = sigma(W_{ii} x + b_{ii} + W_{hi} h + b_{hi})
        ft = sigma(W_{if} x + b_{if} + W_{hf} h + b_{hf})
        gt = tanh(W_{ig} x + b_{ig} + W_{hg} h + b_{hg})
        ot = sigma(W_{io} x + b_{io} + W_{ho} h + b_{ho})
        ct = ft * c + it * gt
        ht = ot * tanh(ct)
        */
        size_t hiddenUnits = this->_hiddenUnits;
        size_t stackHeight = 4; // LSTM has 4 stacked weights for (input, forget, cell, output).
        VectorType inputVector(this->_input.GetValue());
        size_t numRows = stackHeight * hiddenUnits;
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
        VectorType istack(inputBias); // add input bias
        math::MultiplyScaleAddUpdate(alpha, inputWeights, inputVector, beta, istack);

        // Wh * h + b_h
        VectorType hstack(hiddenBias); // add hidden bias
        math::MultiplyScaleAddUpdate(alpha, hiddenWeights, this->_hiddenState, beta, hstack);

        // 4 slices of the vector representing the LSTM input, forget, cell, output layers.
        auto slice1 = 0;
        auto slice2 = hiddenUnits;
        auto slice3 = 2 * hiddenUnits;
        auto slice4 = 3 * hiddenUnits;

        // inputGate = sigma(W_{ii} x + b_{ii} + W_{hi} h + b_{hi})
        VectorType inputGate(hiddenUnits);
        inputGate.CopyFrom(istack.GetSubVector(slice1, hiddenUnits));
        inputGate += hstack.GetSubVector(slice1, hiddenUnits);
        this->_recurrentActivation.Apply(inputGate);

        // forgetGate = sigma(W_{if} x + b_{if} + W_{hf} h + b_{hf})
        VectorType forgetGate(hiddenUnits);
        forgetGate.CopyFrom(istack.GetSubVector(slice2, hiddenUnits));
        forgetGate += hstack.GetSubVector(slice2, hiddenUnits);
        this->_recurrentActivation.Apply(forgetGate);

        // cellGate = tanh(W_{ig} x + b_{ig} + W_{hg} h + b_{hg})
        VectorType cellGate(hiddenUnits);
        cellGate.CopyFrom(istack.GetSubVector(slice3, hiddenUnits));
        cellGate += hstack.GetSubVector(slice3, hiddenUnits);
        this->_activation.Apply(cellGate);

        // outputGate = sigma(W_{io} x + b_{io} + W_{ho} h + b_{ho})
        VectorType outputGate(hiddenUnits);
        outputGate.CopyFrom(istack.GetSubVector(slice4, hiddenUnits));
        outputGate += hstack.GetSubVector(slice4, hiddenUnits);
        this->_recurrentActivation.Apply(outputGate);

        // ct = ft * c + it * gt
        for (size_t i = 0; i < hiddenUnits; i++)
        {
            auto ft = forgetGate[i];
            auto ct = this->_cellState[i];
            auto it = inputGate[i];
            auto gt = cellGate[i];
            auto newValue = ft * ct + it * gt;
            this->_cellState[i] = newValue;
        }

        // ht = ot * tanh(ct)
        VectorType temp(hiddenUnits);
        temp.CopyFrom(this->_cellState);
        this->_activation.Apply(temp);
        ElementwiseMultiplySet(outputGate, temp, this->_hiddenState);

        if (this->ShouldReset())
        {
            const_cast<LSTMNode<ValueType>*>(this)->Reset();
        }

        // copy to output
        this->_output.SetOutput(this->_hiddenState.ToArray());
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::Reset()
    {
        this->_cellState.Reset();
        this->_hiddenState.Reset();
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        /*
        it = sigma(W_{ii} x + b_{ii} + W_{hi} h + b_{hi})
        ft = sigma(W_{if} x + b_{if} + W_{hf} h + b_{hf})
        gt = tanh(W_{ig} x + b_{ig} + W_{hg} h + b_{hg})
        ot = sigma(W_{io} x + b_{io} + W_{ho} h + b_{ho})
        ct = ft * c + it * gt
        ht = ot * tanh(ct)
        */
        const int hiddenUnits = static_cast<int>(this->_hiddenUnits);
        const int inputSize = static_cast<int>(this->input.Size());
        size_t stackHeight = 4; // LSTM has 4 stacked weights for (input, forget, cell, output).

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

        // Allocate global buffer for cell state
        auto cellStateVariable = module.Variables().AddVectorVariable<ValueType>(emitters::VariableScope::global, hiddenUnits);
        auto cellStateValue = module.EnsureEmitted(*cellStateVariable);
        auto cellStatePointer = function.PointerOffset(cellStateValue, 0); // convert "global variable" to a pointer
        auto cellState = function.LocalArray(cellStatePointer);
        auto& prevCellState = cellState;

        // Allocate local variables
        const size_t stackSize = hiddenUnits * stackHeight;
        auto istack = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), stackSize));
        auto hstack = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), stackSize));
        auto inputGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto forgetGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto cellGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto outputGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));

        auto alpha = static_cast<ValueType>(1.0); // GEMV scaling of the matrix multipication
        auto beta = static_cast<ValueType>(1.0); // GEMV scaling of the bias addition

        // W_i * x + b_i, one matrix multiplication for all 4 gates (input, forget, cell, output)
        function.MemoryCopy<ValueType>(inputBias, istack, stackSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(stackSize, inputSize, alpha, inputWeights, inputSize, input, 1, beta, istack, 1);

        // W_h * h + b_h
        function.MemoryCopy<ValueType>(hiddenBias, hstack, stackSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(stackSize, hiddenUnits, alpha, hiddenWeights, hiddenUnits, prevHiddenState, 1, beta, hstack, 1);

        // the weights are stacked in 4 slices for (input, forget, cell, output).
        auto istack_slice0 = istack;
        auto istack_slice1 = function.LocalArray(function.PointerOffset(istack, function.LocalScalar(hiddenUnits)));
        auto istack_slice2 = function.LocalArray(function.PointerOffset(istack, function.LocalScalar(hiddenUnits * 2)));
        auto istack_slice3 = function.LocalArray(function.PointerOffset(istack, function.LocalScalar(hiddenUnits * 3)));

        auto hstack_slice0 = hstack;
        auto hstack_slice1 = function.LocalArray(function.PointerOffset(hstack, function.LocalScalar(hiddenUnits)));
        auto hstack_slice2 = function.LocalArray(function.PointerOffset(hstack, function.LocalScalar(hiddenUnits * 2)));
        auto hstack_slice3 = function.LocalArray(function.PointerOffset(hstack, function.LocalScalar(hiddenUnits * 3)));

        // input_gate = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            inputGate[i] = istack_slice0[i] + hstack_slice0[i];
        });
        this->ApplyActivation(function, this->_recurrentActivation, inputGate, hiddenUnits);

        // forget_gate = sigma(W_{if} x + b_{if} + W_{hf} h + b_{hf})
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            forgetGate[i] = istack_slice1[i] + hstack_slice1[i];
        });
        this->ApplyActivation(function, this->_recurrentActivation, forgetGate, hiddenUnits);

        // cell_gate = tanh(W_{ig} x + b_{ig} + W_{hg} h + b_{hg})
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            cellGate[i] = istack_slice2[i] + hstack_slice2[i];
        });
        this->ApplyActivation(function, this->_activation, cellGate, hiddenUnits);

        // output_gate = sigma(W_{io} x + b_{io} + W_{ho} h + b_{ho})
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            outputGate[i] = istack_slice3[i] + hstack_slice3[i];
        });
        this->ApplyActivation(function, this->_recurrentActivation, outputGate, hiddenUnits);

        // cellState = forget_gate * cellState + input_gate * cell_gate
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            auto ft = forgetGate[i];
            auto ct = prevCellState[i];
            auto it = inputGate[i];
            auto gt = cellGate[i];
            auto newValue = ft * ct + it * gt;
            cellState[i] = newValue;
        });

        // newHiddenState = output_gate * tanh(ct), we'll reuse inputGate local variable to compile tanh(ct)
        function.MemoryCopy<ValueType>(cellState, inputGate, hiddenUnits);
        this->ApplyActivation(function, this->_activation, inputGate, hiddenUnits);

        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            auto tan_ct = inputGate[i];
            auto ot = outputGate[i];
            auto newValue = ot * tan_ct;
            hiddenState[i] = newValue;
        });

        // Copy hidden state to the output.
        function.MemoryCopy<ValueType>(hiddenState, output, hiddenUnits);

        // Add the internal reset function
        std::string resetFunctionName = compiler.GetGlobalName(*this, "LSTMNodeReset");
        emitters::IRFunctionEmitter& resetFunction = module.BeginResetFunction(resetFunctionName);
        auto resetHiddenState = resetFunction.LocalArray(hiddenStateValue);
        auto resetCellState = resetFunction.LocalArray(cellStateValue);
        resetFunction.MemorySet<ValueType>(resetHiddenState, 0, function.Literal<uint8_t>(0), hiddenUnits);
        resetFunction.MemorySet<ValueType>(resetCellState, 0, function.Literal<uint8_t>(0), hiddenUnits);
        // resetFunction.Print("### LSTM Node was reset\n"); // this is a handy way to debug whether the VAD node is working or not.
        module.EndResetFunction();

        // if the reset trigger drops to zero then it means it is time to reset this node, but only do this when signal transitions from 1 to 0
        // Allocate global variable to hold the previous trigger value so we can detect the change in state.
        auto lastSignal = module.Global<int>(compiler.GetGlobalName(*this, "lastSignal"), 0);
        auto lastSignalValue = function.LocalScalar(function.Load(lastSignal));
        auto resetTriggerValue = function.LocalScalar(function.Load(resetTrigger));
        function.If((resetTriggerValue == 0) && (lastSignalValue == 1), [resetFunctionName](emitters::IRFunctionEmitter& fn) {
            fn.Call(resetFunctionName);
        });
        function.Store(lastSignal, resetTriggerValue);
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        RNNNode<ValueType>::WriteToArchive(archiver);
        _recurrentActivation.WriteToArchive(archiver);
    }

    template <typename ValueType>
    void LSTMNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        RNNNode<ValueType>::ReadFromArchive(archiver);
        _recurrentActivation.ReadFromArchive(archiver);

        this->_cellState.Resize(this->_hiddenUnits);
    }

    // Explicit specialization
    template class LSTMNode<float>;
    template class LSTMNode<double>;

} // nodes
} // ell
