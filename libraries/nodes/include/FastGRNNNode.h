////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FastGRNNNode.h (nodes)
//  Authors:  Yash Gaurkar, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ActivationFunctions.h"

#include <predictors/neural/include/Activation.h>

#include <emitters/include/LLVMUtilities.h>

#include <model/include/CompilableCodeNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>

#include <utilities/include/StringUtil.h>

#include <value/include/Vector.h>

#include <string>

namespace ell
{
namespace nodes
{
    ///<summary> The RNNNode implements simple recurrent neural network. See  See http://colah.github.io/posts/2015-08-Understanding-LSTMs/ </summary>
    template <typename ElementType>
    class FastGRNNNode : public model::CompilableCodeNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* resetTriggerPortName = "resetTrigger";
        static constexpr const char* W1PortName = "inputWeights1";
        static constexpr const char* W2PortName = "inputWeights2";
        static constexpr const char* U1PortName = "updateWeights1";
        static constexpr const char* U2PortName = "updateWeights2";
        static constexpr const char* biasGatePortName = "biasGate";
        static constexpr const char* biasUpdatePortName = "biasUpdate";
        static constexpr const char* zetaPortName = "zeta";
        static constexpr const char* nuPortName = "nu";
        const model::InputPort<ElementType>& input = _input;
        const model::InputPort<ElementType>& W1 = _inputWeights1;
        const model::InputPort<ElementType>& W2 = _inputWeights2;
        const model::InputPort<ElementType>& U1 = _updateWeights1;
        const model::InputPort<ElementType>& U2 = _updateWeights2;
        const model::InputPort<ElementType>& biasGate = _biasGate;
        const model::InputPort<ElementType>& bias_update = _biasUpdate;
        const model::InputPort<ElementType>& zeta = _zeta;
        const model::InputPort<ElementType>& nu = _nu;
        const model::InputPortBase& resetTrigger = _resetTrigger;
        const model::OutputPort<ElementType>& output = _output;
        /// @}

        using ActivationType = predictors::neural::Activation<ElementType>;

        /// <summary> Default contructor. </summary>
        FastGRNNNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="resetTrigger"> Port elements for the reset trigger, when the trigger goes from 1 to 0 the FastGRNN state is reset. </param>
        /// <param name="hiddenUnits"> The number of hidden units. </param>
        /// <param name="inputWeights"> The weights to be applied to the input, already transposed so it is hidden_units x input_size. </param>
        /// <param name="hiddenWeights"> The weights to be applied to the hidden state (hidden_units x hidden_units). </param>
        /// <param name="biasGate"> The bias to be applied to the input. </param>
        /// <param name="biasUpdate"> The bias to be applied to the hidden state. </param>
        /// <param name="zeta"> The first learnable scalar applied to 1 - zt gate. </param>
        /// <param name="nu"> The second learnable scalar added to the zeta(1-zt) term. </param>
        /// <param name="gateActivation"> The activation function applied to the gate. </param>
        /// <param name="updateActivation"> The activation function applied to the state update. </param>
        FastGRNNNode(const model::OutputPort<ElementType>& input,
                     const model::OutputPortBase& resetTrigger,
                     size_t hiddenUnits,
                     size_t wRank,
                     size_t uRank,
                     const model::OutputPort<ElementType>& inputWeights1,
                     const model::OutputPort<ElementType>& inputWeights2,
                     const model::OutputPort<ElementType>& hiddenWeights1,
                     const model::OutputPort<ElementType>& hiddenWeights2,
                     const model::OutputPort<ElementType>& biasGate,
                     const model::OutputPort<ElementType>& biasUpdate,
                     const model::OutputPort<ElementType>& zeta,
                     const model::OutputPort<ElementType>& nu,
                     const ActivationType& gateActivation,
                     const ActivationType& updateActivation);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("FastGRNNNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Define(ell::value::FunctionDeclaration& fn) override;
        void DefineReset(ell::value::FunctionDeclaration& fn) override;
        void ClearHiddenState();

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        void Copy(model::ModelTransformer& transformer) const override;
        void ValidateWeights() const;

        model::InputPort<ElementType> _input;
        model::InputPortBase _resetTrigger;
        size_t _hiddenUnits;
        size_t _wRank;
        size_t _uRank;
        model::InputPort<ElementType> _inputWeights1;
        model::InputPort<ElementType> _inputWeights2;
        model::InputPort<ElementType> _updateWeights1;
        model::InputPort<ElementType> _updateWeights2;
        model::InputPort<ElementType> _biasGate;
        model::InputPort<ElementType> _biasUpdate;
        model::InputPort<ElementType> _zeta;
        model::InputPort<ElementType> _nu;
        model::OutputPort<ElementType> _output;
        ActivationType _gateActivation;
        ActivationType _updateActivation;

    private:
        value::Vector _hiddenState;
        value::Scalar _lastResetValue;
    };
} // namespace nodes
} // namespace ell
