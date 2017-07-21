////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorProductNode.h (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// math
#include "Matrix.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a Matrix-Vector product. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    ///
    template <typename ValueType, math::MatrixLayout layout>
    class MatrixVectorProductNode : public model::Node
    {
        public:
            /// @name Input and Output Ports
            /// @{
            static constexpr const char* inputPortName = "input";
            static constexpr const char* outputPortName = "output";
            const model::InputPort<ValueType>& input = _input;
            const model::OutputPort<ValueType>& output = _output;
            /// @}

            /// <summary> Default Constructor </summary>
            MatrixVectorProductNode();

            /// <summary> Constructor </summary>
            ///
            /// <param name="input"> The signal to predict from </param>
            /// <param name="predictor"> The projection matrix </param>
            MatrixVectorProductNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& w);

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return "MatrixVectorProductNode"; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
            virtual void Copy(model::ModelTransformer& transformer) const override;

            /// <summary> Refines this node in the model being constructed by the transformer </summary>
            virtual bool Refine(model::ModelTransformer& transformer) const override;

        protected:
            virtual void Compute() const override;
            virtual void WriteToArchive(utilities::Archiver& archiver) const override;
            virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            // Inputs
            model::InputPort<ValueType> _input;

            // Output
            model::OutputPort<ValueType> _output;

            // Projection matrix
            math::Matrix<ValueType, layout> _w;
    };

    /// <summary> Adds a Matrix vector product node to a model transformer. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="w"> The Matrix. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer);
}
}

#include "../tcc/MatrixVectorProductNode.tcc"