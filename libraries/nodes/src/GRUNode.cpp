////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRUNode.cpp (nodes)
//  Authors:  Chuck Jacobs, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GRUNode.h"

#include <math/include/MatrixOperations.h>

#include <utilities/include/Exception.h>

namespace ell
{
namespace nodes
{
    //
    // GRUNode
    //
    template <typename ValueType>
    GRUNode<ValueType>::GRUNode() :
        LSTMNode<ValueType>()
    {
    }

    template <typename ValueType>
    GRUNode<ValueType>::GRUNode(const model::OutputPort<ValueType>& input,
                                const model::OutputPortBase& resetTrigger,
                                size_t hiddenUnits,
                                const model::OutputPort<ValueType>& inputWeights,
                                const model::OutputPort<ValueType>& hiddenWeights,
                                const model::OutputPort<ValueType>& inputBias,
                                const model::OutputPort<ValueType>& hiddenBias,
                                const ActivationType& activation,
                                const ActivationType& recurrentActivation,
                                bool validateWeights) :
        LSTMNode<ValueType>(input, resetTrigger, hiddenUnits, inputWeights, hiddenWeights, inputBias, hiddenBias, activation, recurrentActivation, false)
    {
        if (validateWeights)
        {
            size_t stackHeight = 3; // GRU has 3 stacked weights for (input, reset, hidden).
            size_t numRows = stackHeight * hiddenUnits;
            size_t numColumns = input.Size();

            if (inputWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The GRUNode input weights are the wrong size, found %zu but expecting %zu", inputWeights.Size(), numRows * numColumns));
            }

            numColumns = hiddenUnits;
            if (hiddenWeights.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The GRUNode hidden weights are the wrong size, found %zu but expecting %zu", hiddenWeights.Size(), numRows * numColumns));
            }

            if (inputBias.Size() != numRows)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The GRUNode input bias vector is the wrong size, found %zu but expecting %zu", inputBias.Size(), numRows));
            }

            if (hiddenBias.Size() != numRows)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The GRUNode hidden bias vector is the wrong size, found %zu but expecting %zu", hiddenBias.Size(), numRows));
            }
        }
    }

    template <typename ValueType>
    void GRUNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->_input);
        const auto& newResetTrigger = transformer.GetCorrespondingInputs(this->_resetTrigger);
        const auto& newInputWeights = transformer.GetCorrespondingInputs(this->_inputWeights);
        const auto& newHiddenWeights = transformer.GetCorrespondingInputs(this->_hiddenWeights);
        const auto& newInputBias = transformer.GetCorrespondingInputs(this->_inputBias);
        const auto& newHiddenBias = transformer.GetCorrespondingInputs(this->_hiddenBias);
        auto newNode = transformer.AddNode<GRUNode>(newInput, newResetTrigger, this->_hiddenUnits, newInputWeights, newHiddenWeights, newInputBias, newHiddenBias, this->_activation, this->_recurrentActivation);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void GRUNode<ValueType>::Compute() const
    {
        using ConstMatrixReferenceType = math::ConstRowMatrixReference<ValueType>;
        /*
        h = previous hidden state
        rt = sigma(W_{ ir } x + b_{ ir } + W_{ hr } h + b_{ hr })
        zt = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
        nt = tanh(W_{ in } x + b_{ in } + rt * (W_{ hn } h + b_{ hn }))
        ht = (1 - zt) * nt + zt * h
        */
        size_t hiddenUnits = this->_hiddenUnits;
        size_t stackHeight = 3; // GRU has 3 stacked weights for (input, reset, hidden)
        VectorType inputVector = this->_input.GetValue();
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
        auto beta = alpha; // GEMV scale bias

        // W_i * x + b_i
        VectorType istack(inputBias); // add input bias
        math::MultiplyScaleAddUpdate(alpha, inputWeights, inputVector, beta, istack);

        // W_h * h + b_h
        VectorType hstack(hiddenBias); // add hidden bias
        math::MultiplyScaleAddUpdate(alpha, hiddenWeights, this->_hiddenState, beta, hstack);

        // the weights are stacked in 3 slices for (input, reset, hidden).
        size_t slice1 = 0;
        size_t slice2 = hiddenUnits;
        size_t slice3 = 2 * hiddenUnits;

        // input_gate = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
        VectorType input_gate(hiddenUnits);
        input_gate.CopyFrom(istack.GetSubVector(slice1, hiddenUnits));
        input_gate += hstack.GetSubVector(slice1, hiddenUnits);
        this->_recurrentActivation.Apply(input_gate);

        // reset_gate = sigma(W_{ ir } x + b_{ ir } + W_{ hr } h + b_{ hr })
        VectorType reset_gate(hiddenUnits);
        reset_gate.CopyFrom(istack.GetSubVector(slice2, hiddenUnits));
        reset_gate += hstack.GetSubVector(slice2, hiddenUnits);
        this->_recurrentActivation.Apply(reset_gate);

        // hidden_gate = tanh(W_{ in } x + b_{ in } + reset_gate * (W_{ hn } h + b_{ hn }))
        VectorType hidden_gate(hiddenUnits);
        hidden_gate.CopyFrom(hstack.GetSubVector(slice3, hiddenUnits));
        ElementwiseMultiplySet(hidden_gate, reset_gate, hidden_gate);
        hidden_gate += istack.GetSubVector(slice3, hiddenUnits);
        this->_activation.Apply(hidden_gate);

        // ht = (1 - input_gate) * hidden_gate + input_gate * h
        //    = hidden_gate - input_gate * hidden_gate + input_gate * h
        //    = hidden_gate + input_gate (h - hidden_gate )
        this->_hiddenState -= hidden_gate;
        ElementwiseMultiplySet(this->_hiddenState, input_gate, this->_hiddenState);
        this->_hiddenState += hidden_gate;

        if (this->ShouldReset())
        {
            const_cast<GRUNode<ValueType>*>(this)->Reset();
        }

        this->_output.SetOutput(this->_hiddenState.ToArray());
    }

    template <typename ValueType>
    void GRUNode<ValueType>::Reset()
    {
        LSTMNode<ValueType>::Reset();
    }

    // Notation:
    // The notation in the comments is adapted from the explanation at http://colah.github.io/posts/2015-08-Understanding-LSTMs/ with the addition of
    // a separate input and hidden bias.  In the the weights and biases are stacked in the order (reset, hidden, update).

    //
    template <typename ValueType>
    void GRUNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const int hiddenUnits = static_cast<int>(this->_hiddenUnits);
        const int inputSize = static_cast<int>(this->input.Size());
        const int outputSize = static_cast<int>(this->_hiddenUnits);

        // Get LLVM references for all node inputs
        auto input = compiler.EnsurePortEmitted(this->input);
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
        const size_t stackSize = hiddenUnits * 3;
        auto istack = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), stackSize));
        auto hstack = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), stackSize));
        auto inputGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto resetGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));
        auto hiddenGate = function.LocalArray(function.Variable(emitters::GetVariableType<ValueType>(), hiddenUnits));

        auto alpha = static_cast<ValueType>(1.0); // GEMV scaling of the matrix multipication
        auto beta = static_cast<ValueType>(1.0); // GEMV scaling of the bias addition

        // W_i * x + b, one matrix multiplication for all 3 gates (input,reset,hidden)
        function.MemoryCopy<ValueType>(inputBias, istack, stackSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(stackSize, inputSize, alpha, inputWeights, inputSize, input, 1, beta, istack, 1);

        // W_h * h + b
        function.MemoryCopy<ValueType>(hiddenBias, hstack, stackSize); // Copy bias values into output so GEMM call accumulates them
        function.CallGEMV(stackSize, hiddenUnits, alpha, hiddenWeights, hiddenUnits, hiddenState, 1, beta, hstack, 1);

        // the weights are stacked in 3 slices for (input, reset, hidden).
        auto istack_slice0 = istack;
        auto istack_slice1 = function.LocalArray(function.PointerOffset(istack, function.LocalScalar(hiddenUnits)));
        auto istack_slice2 = function.LocalArray(function.PointerOffset(istack, function.LocalScalar(hiddenUnits * 2)));
        auto hstack_slice0 = hstack;
        auto hstack_slice1 = function.LocalArray(function.PointerOffset(hstack, function.LocalScalar(hiddenUnits)));
        auto hstack_slice2 = function.LocalArray(function.PointerOffset(hstack, function.LocalScalar(hiddenUnits * 2)));

        // input_gate = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            inputGate[i] = istack_slice0[i] + hstack_slice0[i];
        });
        this->ApplyActivation(function, this->_recurrentActivation, inputGate, hiddenUnits);

        // reset_gate = sigma(W_{ ir } x + b_{ ir } + W_{ hr } h + b_{ hr })
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            resetGate[i] = istack_slice1[i] + hstack_slice1[i];
        });
        this->ApplyActivation(function, this->_recurrentActivation, resetGate, hiddenUnits);

        // hidden_gate = tanh(W_{ in } x + b_{ in } + reset_gate * (W_{ hn } h + b_{ hn }))
        function.For(hiddenUnits, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            hiddenGate[i] = istack_slice2[i] + resetGate[i] * hstack_slice2[i];
        });
        this->ApplyActivation(function, this->_activation, hiddenGate, hiddenUnits);

        //ht = (1 - input_gate) * hidden_gate + input_gate * h
        //   = hidden_gate - input_gate * hidden_gate + input_gate * h
        //   = hidden_gate + input_gate (h - hidden_gate )

        function.For(outputSize, [=](emitters::IRFunctionEmitter& fn, emitters::IRLocalScalar i) {
            auto z_i = inputGate[i];
            auto n_i = hiddenGate[i];
            auto h_i = prevHiddenState[i];
            auto newValue = n_i + z_i * (h_i - n_i);
            hiddenState[i] = newValue;
        });

        // Copy hidden state to the output.
        function.MemoryCopy<ValueType>(hiddenState, output, hiddenUnits);

        // Add the internal reset function
        std::string resetFunctionName = compiler.GetGlobalName(*this, "GRUNodeReset");
        emitters::IRFunctionEmitter& resetFunction = module.BeginResetFunction(resetFunctionName);
        auto resetHiddenState = resetFunction.LocalArray(hiddenStateValue);
        resetFunction.MemorySet<ValueType>(resetHiddenState, 0, function.Literal<uint8_t>(0), outputSize);
        //resetFunction.Print("### GRU Node was reset\n"); // this is a handy way to debug whether the VAD node is working or not.
        module.EndResetFunction();

        this->CompileReset(compiler, function, resetFunctionName);
    }

    // Explicit specialization
    template class GRUNode<float>;
    template class GRUNode<double>;

} // namespace nodes
} // namespace ell
