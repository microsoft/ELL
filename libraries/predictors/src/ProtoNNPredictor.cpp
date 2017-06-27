////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNPredictor.cpp (predictors)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProtoNNPredictor.h"

// stl
#include <memory>

namespace ell
{
namespace predictors
{
    ProtoNNPredictor::ProtoNNPredictor()
        : _dimension(0), _W(0, 0), _B(0, 0), _Z(0, 0), _gamma(0)
    {
    }

    ProtoNNPredictor::ProtoNNPredictor(size_t dimension, size_t projectedDimension, size_t numPrototypes, size_t numLabels, double gamma)
        : _dimension(dimension), _W(projectedDimension, dimension), _B(projectedDimension, numPrototypes), _Z(numLabels, numPrototypes), _gamma(gamma)
    {
    }

    void ProtoNNPredictor::Reset()
    {
        _dimension = 0;
        _W.Reset();
        _B.Reset();
        _Z.Reset();
        _gamma = 0.0;
    }

    math::ColumnVector<double> ProtoNNPredictor::GetLabelScores(const DataVectorType& inputVector) const
    {
        // Projection
        math::ColumnVector<double> data(inputVector.ToArray());
        math::ColumnVector<double> projectedInput(GetProjectedDimension());
        math::Operations::Multiply(1.0, _W, data, 0.0, projectedInput);

        // Similarity to each prototype
        auto numPrototypes = GetNumPrototypes();
        math::ColumnVector<double> similarityToPrototypes(numPrototypes);
        auto prototypes = GetPrototypes();
        auto gammaVal = GetGamma();

        for (size_t i = 0; i < numPrototypes; i++)
        {
            math::ColumnVector<double> prototype(prototypes.GetColumn(i).ToArray());
            prototype -= projectedInput;
            auto prototypeDistance = prototype.Norm2();
            auto similarity = std::exp(-1 * gammaVal * gammaVal * prototypeDistance * prototypeDistance);
            similarityToPrototypes[i] = similarity;
        }

        // Get the prediction label
        math::ColumnVector<double> labels(GetNumLabels());
        math::Operations::Multiply(1.0, GetLabelEmbeddings(), similarityToPrototypes, 0.0, labels);

        return labels;
    }

    ProtoNNPrediction ProtoNNPredictor::Predict(const DataVectorType& inputVector) const
    {
        auto labels = GetLabelScores(inputVector);
        auto maxElement = std::max_element(labels.GetDataPointer(), labels.GetDataPointer() + labels.Size());
        auto maxLabelIndex = maxElement - labels.GetDataPointer();

        ProtoNNPrediction prediction{ *maxElement, (size_t)maxLabelIndex };

        return prediction;
    }

    void ProtoNNPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["dim"] << _dimension;
        archiver["gamma"] << _gamma;
        WriteMatrixToArchive(archiver, "w_rows", "w_columns", "w_data", _W);
        WriteMatrixToArchive(archiver, "b_rows", "b_columns", "b_data", _B);
        WriteMatrixToArchive(archiver, "z_rows", "z_columns", "z_data", _Z);
    }

    void ProtoNNPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["dim"] >> _dimension;
        archiver["gamma"] >> _gamma;
        _W = ReadMatrixFromArchive(archiver, "w_rows", "w_columns", "w_data");
        _B = ReadMatrixFromArchive(archiver, "b_rows", "b_columns", "b_data");
        _Z = ReadMatrixFromArchive(archiver, "z_rows", "z_columns", "z_data");
    }

    void ProtoNNPredictor::WriteMatrixToArchive(utilities::Archiver& archiver, std::string rowLabel, std::string colLabel, std::string dataLabel, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> M)
    {
        archiver[rowLabel] << M.NumRows();
        archiver[colLabel] << M.NumColumns();
        std::vector<double> temp;
        temp.assign(M.GetDataPointer(), M.GetDataPointer() + (size_t)(M.NumRows() * M.NumColumns()));
        archiver[dataLabel] << temp;
    }

    math::ColumnMatrix<double> ProtoNNPredictor::ReadMatrixFromArchive(utilities::Unarchiver& archiver, std::string rowLabel, std::string colLabel, std::string dataLabel)
    {
        size_t w_rows = 0;
        size_t w_columns = 0;
        archiver[rowLabel] >> w_rows;
        archiver[colLabel] >> w_columns;
        std::vector<double> temp;
        archiver[dataLabel] >> temp;
        return math::ColumnMatrix<double>(w_rows, w_columns, temp);
    }
}
}
