////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RNNNode.h (nodes)
//  Authors:  James Devine, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ActivationFunctions.h"

#include <predictors/neural/include/Activation.h>

#include <emitters/include/LLVMUtilities.h>

#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/PortElements.h>

#include <utilities/include/StringUtil.h>

#include <string>

namespace ell
{
namespace nodes
{
    ///<summary> The RNNNode implements simple recurrent neural network. See  See http://colah.github.io/posts/2015-08-Understanding-LSTMs/ </summary>
    template <typename ValueType>
    class RNNNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* resetTriggerPortName = "resetTrigger";
        static constexpr const char* inputWeightsPortName = "inputWeights";
        static constexpr const char* hiddenWeightsPortName = "hiddenWeights";
        static constexpr const char* inputBiasPortName = "inputBias";
        static constexpr const char* hiddenBiasPortName = "hiddenBias";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& inputWeights = _inputWeights;
        const model::InputPort<ValueType>& hiddenWeights = _hiddenWeights;
        const model::InputPort<ValueType>& inputBias = _inputBias;
        const model::InputPort<ValueType>& hiddenBias = _hiddenBias;
        const model::InputPort<int>& resetTrigger = _resetTrigger;
        const model::OutputPort<ValueType>& output = _output;
        using ActivationType = predictors::neural::Activation<ValueType>;
        /// @}

        /// <summary> Default contructor. </summary>
        RNNNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="resetTrigger"> Port elements for the reset trigger. </param>
        /// <param name="hiddenUnits"> The number of hidden units. </param>
        /// <param name="inputWeights"> The weights to be applied to the input. </param>
        /// <param name="hiddenWeights"> The weights to be applied to the hidden state. </param>
        /// <param name="inputBias"> The bias to be applied to the input. </param>
        /// <param name="hiddenBias"> The bias to be applied to the hidden state. </param>
        /// <param name="activation"> The activation function. </param>
        /// <param name="recurrentActivation"> The recurrent activation function. </param>
        /// <param name="validateWeights"> Whether to check the size of the weights. </param>
        RNNNode(const model::OutputPort<ValueType>& input,
                const model::OutputPort<int>& resetTrigger,
                size_t hiddenUnits,
                const model::OutputPort<ValueType>& inputWeights,
                const model::OutputPort<ValueType>& hiddenWeights,
                const model::OutputPort<ValueType>& inputBias,
                const model::OutputPort<ValueType>& hiddenBias,
                const ActivationType& activation,
                bool validateWeights = true);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("RNNNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Resets any state on the node, if any </summary>
        void Reset() override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input;
        model::InputPort<int> _resetTrigger;
        size_t _hiddenUnits;
        model::InputPort<ValueType> _inputWeights;
        model::InputPort<ValueType> _hiddenWeights;
        model::InputPort<ValueType> _inputBias;
        model::InputPort<ValueType> _hiddenBias;
        model::OutputPort<ValueType> _output;
        ActivationType _activation;

        void ApplySoftmax(emitters::IRFunctionEmitter& function, emitters::LLVMValue data, size_t dataLength);

        void ApplyActivation(emitters::IRFunctionEmitter& function, const ActivationType& activation, emitters::LLVMValue data, size_t dataLength);

        using VectorType = math::ColumnVector<ValueType>;

        // Hidden state for compute
        mutable VectorType _hiddenState;

        bool ShouldReset() const;

    private:
        mutable int _lastResetValue;
    };
} // namespace nodes
} // namespace ell
