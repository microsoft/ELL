////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRULayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "GRULayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net GRULayer. </summary>
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    class GRULayerNode : public NeuralNetworkLayerNode<GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>,
                                                       predictors::neural::GRULayer<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>,
                                                       ValueType>
    {
    public:
        using LayerType = predictors::neural::GRULayer<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>;
        using BaseType = NeuralNetworkLayerNode<GRULayerNode<ValueType, ActivationFunctionType, RecurrentActivationFunctionType>, LayerType, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        const model::InputPort<int>& reset = _reset;
        using BaseType::output;
        /// @}

        /// <summary> Default constructor. </summary>
        GRULayerNode();

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> The input signal </param>
        /// <param name="reset"> The reset signal (will reset when this boolean transitions from 1 to 0) </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        GRULayerNode(const model::PortElements<ValueType>& input, const model::PortElements<int>& reset, const LayerType& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, ActivationFunctionType<ValueType>, RecurrentActivationFunctionType<ValueType>>("GRULayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        ///
        /// <returns> true if  this node is able to compile itself to code. </returns>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

        /// <summary> Makes a copy of this node into the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` object currently creating a new model </param>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Reset input signal
        model::InputPort<int> _reset;
    };

    //
    // Implementation: GRUNode
    //
    template <typename ValueType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
    class GRUNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* resetTriggerPortName = "resetTrigger";
        static constexpr const char* updateWeightsPortName = "updateWeights";
        static constexpr const char* resetWeightsPortName = "resetWeights";
        static constexpr const char* hiddenWeightsPortName = "hiddenWeights";
        static constexpr const char* updateBiasPortName = "updateBias";
        static constexpr const char* resetBiasPortName = "resetBias";
        static constexpr const char* hiddenBiasPortName = "hiddenBias";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& updateWeights = _updateWeights;
        const model::InputPort<ValueType>& resetWeights = _resetWeights;
        const model::InputPort<ValueType>& hiddenWeights = _hiddenWeights;
        const model::InputPort<ValueType>& updateBias = _updateBias;
        const model::InputPort<ValueType>& resetBias = _resetBias;
        const model::InputPort<ValueType>& hiddenBias = _hiddenBias;
        const model::InputPort<int>& resetTrigger = _resetTrigger;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default contructor. </summary>
        GRUNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="updateWeights"> The update weights. </param>
        /// <param name="resetWeights"> The reset weights. </param>
        /// <param name="hiddenWeights"> The hidden weights. </param>
        /// <param name="updateBias"> The update bias. </param>
        /// <param name="resetBias"> The reset bias. </param>
        /// <param name="hiddenBias"> The hidden bias. </param>
        /// <param name="resetTrigger"> Port elements for the reset trigger. </param>
        GRUNode(const model::PortElements<ValueType>& input, 
                       const model::PortElements<int>& resetTrigger,
                       const model::PortElements<ValueType>& updateWeights,
                       const model::PortElements<ValueType>& resetWeights,
                       const model::PortElements<ValueType>& hiddenWeights,
                       const model::PortElements<ValueType>& updateBias,
                       const model::PortElements<ValueType>& resetBias,
                       const model::PortElements<ValueType>& hiddenBias,
                       const model::PortMemoryLayout& inputMemoryLayout,
                       const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        ///
        /// <returns> The layout of the output data. </returns>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, ActivationFunctionType<ValueType>, RecurrentActivationFunctionType<ValueType>>("GRUNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Resets any state on the node, if any </summary>
        virtual void Reset() override;

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
        model::InputPort<int> _resetTrigger;
        model::InputPort<ValueType> _updateWeights;
        model::InputPort<ValueType> _resetWeights;
        model::InputPort<ValueType> _hiddenWeights;
        model::InputPort<ValueType> _updateBias;
        model::InputPort<ValueType> _resetBias;
        model::InputPort<ValueType> _hiddenBias;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        template <typename ActivationType>
        void ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength);

    };
}
}
