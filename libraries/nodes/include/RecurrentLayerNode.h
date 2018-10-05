////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RecurrentLayerNode.h (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Activation.h"
#include "CompiledActivationFunctions.h"
#include "NeuralNetworkLayerNode.h"

// emitters
#include "LLVMUtilities.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "RecurrentLayer.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that implements a simple recurrent neural net (RNN). </summary>
    template <typename ValueType>
    class RNNNode : public model::CompilableNode
    {
    public:
        using ActivationType = predictors::neural::Activation<ValueType>;

        /// @name Input and Output Ports
        /// @{
        static constexpr const char* hiddenWeightsPortName = "hiddenWeights";
        static constexpr const char* hiddenBiasPortName = "hiddenBias";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& hiddenWeights = _hiddenWeights;
        const model::InputPort<ValueType>& hiddenBias = _hiddenBias;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default contructor. </summary>
        RNNNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="hiddenWeights"> The weights applied to the hidden layer. </param>
        /// <param name="hiddenBias"> The biases to be applied to the hidden layer. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        RNNNode(const model::OutputPort<ValueType>& input,
                      const model::OutputPort<ValueType>& hiddenWeights,
                      const model::OutputPort<ValueType>& hiddenBias,
                      const ActivationType& activation,
                      const model::PortMemoryLayout& inputMemoryLayout,
                      const model::PortMemoryLayout& outputMemoryLayoutn);

        /// <summary> Gets information about the input memory layout </summary>
        ///
        /// <returns> The layout of the input data. </returns>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        ///
        /// <returns> The layout of the output data. </returns>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("RecurrentNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }

        void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Weights
        model::InputPort<ValueType> _hiddenWeights;

        // Biases
        model::InputPort<ValueType> _hiddenBias;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        ActivationType _activation;

        void ApplySoftmax(emitters::IRFunctionEmitter& function, emitters::LLVMValue data, size_t dataLength);

        void ApplyActivation(emitters::IRFunctionEmitter& function, emitters::LLVMValue data, size_t dataLength);
    };
}
}
