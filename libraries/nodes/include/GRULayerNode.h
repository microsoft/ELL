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
        using BaseType::output;
        /// @}

        GRULayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        GRULayerNode(const model::PortElements<ValueType>& input, const LayerType& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("GRULayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        ///
        /// <returns> true if  this node is able to compile itself to code. </returns>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
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
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        GRUNode(const model::PortElements<ValueType>& input,
                       const model::PortElements<ValueType>& updateWeights,
                       const model::PortElements<ValueType>& resetWeights,
                       const model::PortElements<ValueType>& hiddenWeights,
                       const model::PortElements<ValueType>& updateBias,
                       const model::PortElements<ValueType>& resetBias,
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
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, ActivationFunctionType<ValueType>, RecurrentActivationFunctionType<ValueType>>("GRUNode"); }

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
        model::InputPort<ValueType> _updateWeights;
        model::InputPort<ValueType> _resetWeights;
        model::InputPort<ValueType> _hiddenWeights;
        model::InputPort<ValueType> _updateBias;
        model::InputPort<ValueType> _resetBias;
        model::InputPort<ValueType> _hiddenBias;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        template <typename ActivationType>
        void ApplyActivation(emitters::IRFunctionEmitter& function, ActivationType& activationFunction, llvm::Value* data, size_t dataLength);
    };
}
}
