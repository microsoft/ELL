////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredEuclideanDistanceNode.h (nodes)
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
    /// <summary> A node that represents squared Euclidean distances of input vector to a set of vectors stacked into rows of a matrix. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element and input type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    ///
    template <typename ValueType, math::MatrixLayout layout>
    class SquaredEuclideanDistanceNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SquaredEuclideanDistanceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input vector </param>
        /// <param name="v"> The set of vectors to compute the distance from stacked into a row matrix</param>
        SquaredEuclideanDistanceNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& v);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "SquaredEuclideanDistanceNode"; }
        // static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, layout>("SquaredEuclideanDistanceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }

    private:
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Set of vectors
        math::Matrix<ValueType, layout> _vectorsAsMatrix;
    };

    /// <summary> Adds a SquaredEuclidean node to a model transformer. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="w"> The Matrix. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer);
}
}

#include "../tcc/SquaredEuclideanDistanceNode.tcc"
