////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DotProductNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

// model
#include "BinaryOperationNode.h"
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that takes two vector inputs and returns their dot product </summary>
    template <typename ValueType>
    class DotProductNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DotProductNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input1"> One of the signals to take the dot product of </param>
        /// <param name="input2"> The other signal to take the dot product of </param>
        DotProductNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DotProductNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently copying the model </param>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently refining the model </param>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void CompileDotProductLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileDotProductExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
}
}

#include "../tcc/DotProductNode.tcc"
