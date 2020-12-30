////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingNode.h (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableCodeNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputPort.h>

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <value/include/Vector.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a simple scaling of all input values by a single constant value.
    /// </summary>
    template <typename ValueType>
    class ScalingNode : public model::CompilableCodeNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        ScalingNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to buffer. </param>
        /// <param name="scale"> The scaling factor to apply to the input. </param>
        ScalingNode(const model::OutputPort<ValueType>& input, ValueType scale);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ScalingNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Return the scaling factor </summary>
        ///
        /// <returns> The scaling factor </returns>
        ValueType GetScaleFactor() const { return _scale; }

    protected:
        void Define(value::FunctionDeclaration& fn) override;
        void DefineReset(value::FunctionDeclaration& fn) override;

        bool HasState() const override { return true; } // stored state: windowSize
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // The scale factor
        ValueType _scale;
    };

    /// <summary> Convenience function for adding a buffer node to a model. </summary>
    ///
    /// <param name="input"> The input to the buffer node. </param>
    /// <param name="scale"> The scaling factor to apply to the input. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const model::OutputPort<ValueType>& AddScalingNode(const model::OutputPort<ValueType>& input, ValueType scale);

} // namespace nodes
} // namespace ell
