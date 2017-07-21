////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReshapeImageNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "Port.h"

// stl
#include <algorithm>
#include <array>
#include <numeric>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// A node that reshapes an image into a format suitable for computing convolutions via matrix multiplcation.
    /// Also known as Im2Col.
    /// </summary>
    template <typename ValueType>
    class ReshapeImageNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReshapeImageNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The input image. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input image. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        /// <param name="outputWidth"> The output image width. </param>
        /// <param name="outputHeight"> The output image height. </param>
        ReshapeImageNode(const model::PortElements<ValueType>& input,
                         const PortMemoryLayout& inputMemoryLayout,
                         const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                         size_t outputWidth,
                         size_t outputHeight);

        /// <summary> Gets information about the input memory layout </summary>
        const PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReshapeImageNode"); }

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

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        PortMemoryLayout _inputMemoryLayout;
        predictors::neural::ConvolutionalParameters _convolutionalParameters;
        size_t _outputWidth;
        size_t _outputHeight;
    };
}
}

#include "../tcc/ReshapeImageNode.tcc"
