////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReceptiveFieldMatrixNode.h (nodes)
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
    class ReceptiveFieldMatrixNode : public model::CompilableNode
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
        ReceptiveFieldMatrixNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The input image. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input image. </param>
        /// <param name="filterWidth"> The width of the convolution filters. </param>
        /// <param name="stride"> The distance between filter application. </param>
        /// <param name="convolutionPadding"> The amount of padding to use when performing the convolution. </param>
        /// <param name="dataOrder"> Indicates the order of the dimensions in the data. The first entry is the index of the slowest-incrementing dimension, and the last entry is the index of the fastest-moving dimensions. So, the canonical row, column, channel order is {0, 1, 2}. </param>
        /// <param name="outputWidth"> The output image width. </param>
        /// <param name="outputHeight"> The output image height. </param>
        ReceptiveFieldMatrixNode(const model::PortElements<ValueType>& input,
                                 const model::PortMemoryLayout& inputMemoryLayout,
                                 size_t filterWidth,
                                 size_t stride,
                                 size_t convolutionPadding,
                                 std::array<int, 3> dataOrder, // 0, 1, 2 == rows, columns, channels -- row-major
                                 size_t outputWidth,
                                 size_t outputHeight);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReceptiveFieldMatrixNode"); }

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

        virtual bool HasState() const override { return true; } // stored state: input layout, output shape, conv params

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        size_t _filterWidth;
        size_t _stride;
        size_t _convolutionPadding;
        std::array<int, 3> _dataOrder;
        size_t _outputWidth;
        size_t _outputHeight;
    };
}
}

#include "../tcc/ReceptiveFieldMatrixNode.tcc"
