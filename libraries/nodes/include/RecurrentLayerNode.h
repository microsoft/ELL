////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RecurrentLayerNode.h (nodes)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompiledActivationFunctions.h"
#include "NeuralNetworkLayerNode.h"

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
    /// <summary> A node that wraps a neural net GRULayer. </summary>
    template <typename ValueType, template <typename> class ActivationFunctionType>
    class RecurrentLayerNode : public NeuralNetworkLayerNode<RecurrentLayerNode<ValueType, ActivationFunctionType>, predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>;
        using BaseType = NeuralNetworkLayerNode<RecurrentLayerNode<ValueType, ActivationFunctionType>, predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        RecurrentLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        RecurrentLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::RecurrentLayer<ValueType, ActivationFunctionType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, ActivationFunctionType<ValueType>>("RecurrentLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
    };

    //
    // Implementation: RecurrentNode
    //
    template <typename ValueType, template <typename> class ActivationFunctionType>
    class RecurrentNode : public model::CompilableNode
    {
    public:
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
        RecurrentNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="hiddenWeights"> The weights applied to the hidden layer. </param>
        /// <param name="hiddenBias"> The biases to be applied to the hidden layer. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        RecurrentNode(const model::PortElements<ValueType>& input,
                       const model::PortElements<ValueType>& hiddenWeights,
                       const model::PortElements<ValueType>& hiddenBias,
                       const model::PortMemoryLayout& inputMemoryLayout,
                       const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        ///
        /// <returns> The layout of the input data. </returns>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        ///
        /// <returns> The layout of the output data. </returns>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, ActivationFunctionType<ValueType>>("RecurrentNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Copy(model::ModelTransformer& transformer) const override;
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
        // Input
        model::InputPort<ValueType> _input;

        // Weights
        model::InputPort<ValueType> _hiddenWeights;

        // Biases
        model::InputPort<ValueType> _hiddenBias;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        void ApplySoftmax(emitters::IRFunctionEmitter& function, llvm::Value* data, size_t dataLength);

        template <typename ActivationType>
        void ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength);
    };

}
}
