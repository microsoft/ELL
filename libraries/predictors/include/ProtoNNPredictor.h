////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (ELL)
//  File:     ProtoNNPredictor.h (predictors)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// math
#include "Operations.h"

// datasets
#include "AutoDataVector.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstdint>
#include <memory>

namespace ell
{
namespace predictors
{
    /// <summary> Output of ProtoNN predictor, contains a prediction score and a label (0-based). </summary>
    struct ProtoNNPrediction
    {
        double score;
        size_t label;
    };

    /// <summary> A ProtoNN predictor. </summary>
    ///
    class ProtoNNPredictor : public IPredictor<double>, public utilities::IArchivable
    {
    public:
        /// <summary> Type of the data vector expected by this predictor type. </summary>
        using DataVectorType = data::AutoDataVector;

        /// <summary> Default Constructor. </summary>
        ProtoNNPredictor();

        /// <summary> Constructs an instance of ProtoNN predictor. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        /// <param name="projectedDim"> The projected dimension. </param>
        /// <param name="numPrototypes"> Number of prototypes. </param>
        /// <param name="numLabels"> Number of labels. </param>
        /// <param name="gamma"> The Gamma value. </param>
        ProtoNNPredictor(size_t dim, size_t projectedDim, size_t numPrototypes, size_t numLabels, double gamma);

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The projection matrix. </returns>
        math::ColumnMatrix<double>& GetProjectionMatrix() { return _W; }

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        const math::ColumnMatrix<double>& GetProjectionMatrix() const { return _W; }

        /// <summary> Returns the underlying prototype matrix. </summary>
        ///
        /// <returns> The underlying prototype matrix. </returns>
        math::ColumnMatrix<double>& GetPrototypes() { return _B; }

        /// <summary> Returns the underlying prototype matrix. </summary>
        ///
        /// <returns> The underlying prototype matrix. </returns>
        const math::ColumnMatrix<double>& GetPrototypes() const { return _B; }

        /// <summary> Returns the underlying label embeddings. </summary>
        ///
        /// <returns> The label embeddings. </returns>
        math::ColumnMatrix<double>& GetLabelEmbeddings() { return _Z; }

        /// <summary> Returns the underlying label embeddings. </summary>
        ///
        /// <returns> The label embeddings. </returns>
        const math::ColumnMatrix<double>& GetLabelEmbeddings() const { return _Z; }

        /// <summary> Returns the underlying gamma. </summary>
        ///
        /// <returns> Gamma constant. </returns>
        double& GetGamma() { return _gamma; }

        /// <summary> Returns the underlying gamma. </summary>
        ///
        /// <returns> Gamma constant. </returns>
        double GetGamma() const { return _gamma; }

        /// <summary> Gets the dimension of the ProtoNN predictor. </summary>
        ///
        /// <returns> The dimension. </returns>
        size_t GetDimension() const { return _dimension; }

        /// <summary> Gets the projected dimension of the ProtoNN predictor. </summary>
        ///
        /// <returns> The projected dimension. </returns>
        size_t GetProjectedDimension() const { return _W.NumRows(); }

        /// <summary> Gets the number of prototypes. </summary>
        ///
        /// <returns> The number of prototypes. </returns>
        size_t GetNumPrototypes() const { return _Z.NumColumns(); }

        /// <summary> Gets the number of labels. </summary>
        ///
        /// <returns> The number of labels. </returns>
        size_t GetNumLabels() const { return _Z.NumRows(); }

        /// <summary> Returns the label output of the predictor for a given example. </summary>
        ///
        /// <param name="inputVector"> The data vector. </param>
        ///
        /// <returns> The predicted label with its score. </returns>
        ProtoNNPrediction Predict(const DataVectorType& inputVector) const;

        /// <summary> Resets the projection predictor to the zero projection matrix. </summary>
        void Reset();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ProtoNNPredictor"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        math::ColumnVector<double> GetLabelScores(const DataVectorType& inputVector) const;

        static void WriteMatrixToArchive(utilities::Archiver& archiver, std::string rowLabel, std::string colLabel, std::string dataLabel, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> M);

        static math::ColumnMatrix<double> ReadMatrixFromArchive(utilities::Unarchiver& archiver, std::string rowLabel, std::string colLabel, std::string dataLabel);

        // Input dimension
        size_t _dimension;

        // Projection matrix
        math::ColumnMatrix<double> _W;

        // Prototypes matrix
        math::ColumnMatrix<double> _B;

        // Label embedding matrix
        math::ColumnMatrix<double> _Z;

        // Gamma constant
        double _gamma;
    };
}
}