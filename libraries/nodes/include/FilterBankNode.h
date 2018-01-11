////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FilterBankNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// dsp
#include "FilterBank.h"

// utilities
#include "TypeName.h"
#include "TypeTraits.h"

// stl
#include <cmath>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> 
    /// Base class for nodes that perform elementwise multiply between a set of filters and the input frequency response
    /// </summary>
    template <typename ValueType>
    class FilterBankNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

    protected:
        FilterBankNode(const dsp::TriangleFilterBank& filters);
        FilterBankNode(const model::PortElements<ValueType>& input, const dsp::TriangleFilterBank& filters);
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // Stored state: filters

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

    private:
        // FilterBank
        const dsp::TriangleFilterBank& _filters;
    };

    /// <summary> 
    /// A node that applies a linearly-spaced filter bank to an FFT output 
    /// </summary>
    template <typename ValueType>
    class LinearFilterBankNode : public FilterBankNode<ValueType>
    {
    public:
        /// @name Input and Output Ports
        /// @{
        using FilterBankNode<ValueType>::input;
        using FilterBankNode<ValueType>::output;
        /// @}

        /// <summary> Default Constructor </summary>
        LinearFilterBankNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        LinearFilterBankNode(const model::PortElements<ValueType>& input, const dsp::LinearFilterBank& filters);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("LinearFilterBankNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        using FilterBankNode<ValueType>::_input;
        using FilterBankNode<ValueType>::_output;
        // FilterBank
        dsp::LinearFilterBank _linearFilters;
    };

    /// <summary> 
    /// A node that applies a mel filter bank to an FFT output 
    /// </summary>
    template <typename ValueType>
    class MelFilterBankNode : public FilterBankNode<ValueType>
    {
    public:
        /// @name Input and Output Ports
        /// @{
        using FilterBankNode<ValueType>::input;
        using FilterBankNode<ValueType>::output;
        /// @}

        /// <summary> Default Constructor </summary>
        MelFilterBankNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        MelFilterBankNode(const model::PortElements<ValueType>& input, const dsp::MelFilterBank& filters);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MelFilterBankNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        using FilterBankNode<ValueType>::_input;
        using FilterBankNode<ValueType>::_output;
        // FilterBank
        dsp::MelFilterBank _melFilters;
    };
}
}
