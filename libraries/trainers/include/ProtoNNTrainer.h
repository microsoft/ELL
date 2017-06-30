
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainer.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// trainer
#include "ITrainer.h"

// predictors
#include "ProtoNNPredictor.h"

// parameters
#include "ProtoNNModel.h"

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <memory>
#include <map>

// Matrix
#include <Matrix.h>

namespace ell
{
namespace trainers
{
    class ProtoNNModelParameter;

    /// <summary>
    /// Implements the ProtoNN trainer
    /// </summary>
    class ProtoNNTrainer : public ITrainer<predictors::ProtoNNPredictor>
    {
    public:
        /// <summary> Constructs the ProtoNN trainer. </summary>
        ///
        /// <param name="numFeatures"> The number of training examples. </param>
        /// <param name="numFeatures"> The feature dimension. </param>
        /// <param name="parameters"> The training parameters. </param>
        ProtoNNTrainer(size_t numExamples, size_t numFeatures, const ProtoNNTrainerParameters& parameters);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        virtual void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Trains a ProtoNN model for the given dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void Update() override;

        /// <summary> Returns The ProtoNN predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        virtual const predictors::ProtoNNPredictor& GetPredictor() const override { return _protoNNPredictor; }

    private:
        // The Similarity Kernel.
        math::ColumnMatrix<double> SimilarityKernel(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, math::MatrixReference<double, math::MatrixLayout::columnMajor> WX, const double gamma, const size_t begin, const size_t end, bool recomputeWX = false) const;

        // The Similarity Kernel.
        math::ColumnMatrix<double> SimilarityKernel(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, math::MatrixReference<double, math::MatrixLayout::columnMajor> WX, const double gamma, bool recomputeWX = false) const;

        // The Training Loss.
        double Loss(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference Y, ConstColumnMatrixReference D, const size_t begin, const size_t end) const;

        // The Training Loss.
        double Loss(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference Y, ConstColumnMatrixReference D) const;

        // The Objective function value.
        double ComputeObjective(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, math::MatrixReference<double, math::MatrixLayout::columnMajor> WX, double gamma, bool recomputeWX = false);

        // Performs Accelerated Proximal Gradient w.r.t. input model parameter.
        void AcceleratedProximalGradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ProtoNNParameterIndex parameterIndex, std::function<math::ColumnMatrix<double>(const ConstColumnMatrixReference, const size_t, const size_t)> gradf, std::function<void(math::MatrixReference<double, math::MatrixLayout::columnMajor>)> prox, math::MatrixReference<double, math::MatrixLayout::columnMajor> param, const size_t & epochs, const size_t & n, const size_t & batchSize, const double & eta, const int & eta_update);

        // Optimization using SGD with alternating minimization.
        void SGDWithAlternatingMinimization(ConstColumnMatrixReference X, ConstColumnMatrixReference Y, std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelParameters, double gamma, size_t nIters);

        // Order in which the parameters are optimized
        std::vector<ProtoNNParameterIndex> m_OptimizationOrder{ ProtoNNParameterIndex::W, ProtoNNParameterIndex::Z, ProtoNNParameterIndex::B };

        // Index of the projection parameter
        ProtoNNParameterIndex m_projectionIndex = ProtoNNParameterIndex::W;

        // input dimension
        size_t _dimemsion;

        ProtoNNTrainerParameters _parameters;

        /*std::shared_ptr<const std::shared_ptr<predictors::ProtoNNPredictor>> _protoNNPredictor;*/
        predictors::ProtoNNPredictor _protoNNPredictor;

        math::ColumnMatrix<double> _X;
        math::ColumnMatrix<double> _Y;
    };

    /// <summary>
    /// Implements the base class for Protonn paremeter
    /// </summary>
    class ProtoNNModelParameter
    {
    public:
        /// <summary> Default constructor. </summary>
        ProtoNNModelParameter();

        /// <summary> Instantiates a ProtoNN model parameter. </summary>
        ///
        /// <param name="dimension1"> The first dimension of data matrix. </param>
        /// <param name="dimension2"> The second dimension of data matrix. </param>
        ProtoNNModelParameter(size_t dim1, size_t dim2);

        /// <summary> Get the underlying data matrix. </summary>
        ///
        /// <returns> The underlying data matrix. </returns>
        math::ColumnMatrix<double>& GetData() { return _data; }

        /// <summary> Get the underlying data matrix. </summary>
        ///
        /// <returns> The underlying data matrix. </returns>
        const math::ColumnMatrix<double>& GetData() const { return _data; }

        /// <summary> Parameter gradient. </summary>
        ///
        /// <param name="poolingParameters"> Specifies the interface for gradient computation. </param>
        ///
        /// <returns> The gradient. </returns>
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossType lossType) = 0;

        /// <summary> Instantiates an instance of a pooling layer. </summary>
        ///
        /// <param name="poolingParameters"> Specifies the input and pooling characteristics of the layer. </param>
        ///
        /// <returns> Expected size of the input vector. </returns>
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossType lossType) = 0;

    private:
        // The underlying Parameter matrix
        math::ColumnMatrix<double> _data;
    };

    class Param_W : public ProtoNNModelParameter
    {
    public:
        Param_W(size_t dimension1, size_t dimension2);
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossType lossType) override;
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossType lossType) override;
    };

    class Param_B : public ProtoNNModelParameter
    {
    public:
        Param_B(size_t dimension1, size_t dimension2);
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossType lossType) override;
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossType lossType) override;
    };

    class Param_Z : public ProtoNNModelParameter
    {
    public:
        Param_Z(size_t dimension1, size_t dimension2);
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossType lossType) override;
        virtual math::ColumnMatrix<double> gradient(std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>> &modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossType lossType) override;
    };

    /// <summary> Makes a ProtoNN trainer. </summary>
    ///
    /// <param name="dim"> The dimension. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A ProtoNN trainer </returns>
    std::unique_ptr<trainers::ProtoNNTrainer> MakeProtoNNTrainer(size_t numExamples, size_t numFeatures, const trainers::ProtoNNTrainerParameters& parameters);
}
}
