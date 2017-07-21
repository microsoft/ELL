////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstantNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// emitters
#include "VectorVariable.h"

// predictors
#include "ConstantPredictor.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"
#include "TypeName.h"

// stl
#include <memory>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that contains a constant value. Has no inputs. </summary>
    template <typename ValueType>
    class ConstantNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        ConstantNode();

        /// <summary> Constructor for a scalar constant </summary>
        ///
        /// <param name="value"> The scalar value </param>
        ConstantNode(ValueType value);

        /// Constructor for a vector constant
        ///
        /// <param name="value"> The vector value </param>
        ConstantNode(const std::vector<ValueType>& values);

        /// <summary> Gets the values contained in this node </summary>
        ///
        /// <returns> The values contained in this node </returns>
        const std::vector<ValueType>& GetValues() const { return _values; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ConstantNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual bool HasState() const override { return true; }
        virtual bool ShouldCompileInline() const override { return true; }
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:

        // Output
        model::OutputPort<ValueType> _output;

        // Constant value
        std::vector<ValueType> _values;
    };

    /// <summary> Adds a constant node (which represents a constant predictor) to a model transformer. </summary>
    ///
    /// <param name="input"> The input to the predictor, which is ignored. </param>
    /// <param name="predictor"> The constant predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    ConstantNode<double>* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ConstantPredictor& predictor, model::ModelTransformer& transformer);
}
}

#include "../tcc/ConstantNode.tcc"
