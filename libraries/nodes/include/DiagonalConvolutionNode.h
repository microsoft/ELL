////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DiagonalConvolutionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"
#include "PortMemoryLayout.h"

// predictors
#include "ConvolutionalLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// If diagonal convolution is specified, a ConvolutionalLayerNode will refine
    /// itself into a DiagonalConvolutionNode.
    /// </summary>
    template <typename ValueType>
    class DiagonalConvolutionNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* filterWeightsPortName = "filterWeights";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& filterWeights = _filterWeights;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        DiagonalConvolutionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        DiagonalConvolutionNode(const model::PortElements<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortElements<ValueType>& filterWeights,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                const predictors::neural::ConvolutionalParameters& convolutionalParameters);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Get the parameters used to control convolution. </summary>
        ///
        /// <returns> A ConvolutionalParameters struct. </returns>
        const predictors::neural::ConvolutionalParameters& GetConvolutionalParameters() const { return _convolutionalParameters; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DiagonalConvolutionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node into the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` object currently creating a new model </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        virtual bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout


    private:
        // Input
        model::InputPort<ValueType> _input;
        model::InputPort<ValueType> _filterWeights;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        predictors::neural::ConvolutionalParameters _convolutionalParameters;
    };
}
}
