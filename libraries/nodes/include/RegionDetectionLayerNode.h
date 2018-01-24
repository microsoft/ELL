///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegionDetectionLayer.h (neural)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// predictors
#include "RegionDetectionLayer.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    class RegionDetectionLayerNode : public NeuralNetworkLayerNode<RegionDetectionLayerNode<ValueType>, predictors::neural::RegionDetectionLayer<ValueType>, ValueType>
    {
    public:
        using BaseType = NeuralNetworkLayerNode<RegionDetectionLayerNode<ValueType>, predictors::neural::RegionDetectionLayer<ValueType>, ValueType>;
        using typename BaseType::LayerType;

        RegionDetectionLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        RegionDetectionLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::RegionDetectionLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("RegionDetectionLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
    };

    template <typename ValueType>
    class RegionDetectionNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        RegionDetectionNode();

        RegionDetectionNode(const model::PortElements<ValueType>& input,
                            predictors::neural::RegionDetectionParameters params,
                            const model::PortMemoryLayout& inputMemoryLayout,
                            const model::PortMemoryLayout& outputMemoryLayout);

        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("RegionDetectionNode"); }

        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Copy(model::ModelTransformer& transformer) const override;

        void Compute() const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        void WriteToArchive(utilities::Archiver&) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        void ReadFromArchive(utilities::Unarchiver&) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

    private:
        // Input
        model::InputPort<ValueType> _input;

        predictors::neural::RegionDetectionParameters _params;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;
    };
}
}
