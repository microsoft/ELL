
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

// math
#include "Matrix.h"

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <map>
#include <memory>

namespace ell
{
namespace trainers
{
    typedef math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> ConstColumnMatrixReference;

    enum class ProtoNNParameterIndex
    {
        W = 0,
        B,
        Z
    };

    class ProtoNNModelParameter;

    using ProtoNNModelMap = std::map<ProtoNNParameterIndex, std::shared_ptr<ProtoNNModelParameter>>;

    /// <summary>
    /// Implements the ProtoNN trainer
    /// </summary>
    class ProtoNNTrainer : public ITrainer<predictors::ProtoNNPredictor>
    {
    public:
        /// <summary> Constructs the ProtoNN trainer. </summary>
        ///
        /// <param name="parameters"> The training parameters. </param>
        ProtoNNTrainer(const ProtoNNTrainerParameters& parameters);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Trains a ProtoNN model for the given dataset. </summary>
        void Update() override;

        /// <summary> Returns The ProtoNN predictor. </summary>
        ///
        /// <returns> A shared pointer to the current predictor. </returns>
        const predictors::ProtoNNPredictor& GetPredictor() const override { return _protoNNPredictor; }

    private:
        // Initalize parameters in the first iteration
        void Initialize();

        // The Similarity Kernel.
        math::ColumnMatrix<double> SimilarityKernel(ConstColumnMatrixReference X, math::ColumnMatrixReference<double> WX, const double gamma, const size_t begin, const size_t end, bool recomputeWX = false);

        // The Similarity Kernel.
        math::ColumnMatrix<double> SimilarityKernel(ConstColumnMatrixReference X, math::ColumnMatrixReference<double> WX, const double gamma, bool recomputeWX = false);

        // The Training Loss.
        double Loss(ConstColumnMatrixReference Y, ConstColumnMatrixReference D, const size_t begin, const size_t end);

        // The Training Loss.
        double Loss(ConstColumnMatrixReference Y, ConstColumnMatrixReference D);

        // The Objective function value.
        double ComputeObjective(ConstColumnMatrixReference X, ConstColumnMatrixReference Y, math::ColumnMatrixReference<double> WX, double gamma, bool recomputeWX = false);

        // Performs Accelerated Proximal Gradient w.r.t. input model parameter.
        void AcceleratedProximalGradient(ProtoNNParameterIndex parameterIndex, std::function<math::ColumnMatrix<double>(const ConstColumnMatrixReference, const size_t, const size_t)> gradf, std::function<void(math::MatrixReference<double, math::MatrixLayout::columnMajor>)> prox, math::MatrixReference<double, math::MatrixLayout::columnMajor> param, const size_t& epochs, const size_t& n, const size_t& batchSize, const double& eta, const int& eta_update);

        // Optimization using SGD with alternating minimization.
        void SGDWithAlternatingMinimization(ConstColumnMatrixReference X, ConstColumnMatrixReference Y, double gamma, size_t nIters);

        // Order in which the parameters are optimized
        std::vector<ProtoNNParameterIndex> m_OptimizationOrder{ ProtoNNParameterIndex::W, ProtoNNParameterIndex::Z, ProtoNNParameterIndex::B };

        // Index of the projection parameter
        ProtoNNParameterIndex m_projectionIndex = ProtoNNParameterIndex::W;

        // input dimension
        size_t _dimemsion;

        bool _firstIteration = true;

        ProtoNNTrainerParameters _parameters;

        predictors::ProtoNNPredictor _protoNNPredictor;

        // Map holding the model parameters
        ProtoNNModelMap _modelMap;

        // Maps holding the Trainer knobs
        std::map<ProtoNNParameterIndex, double> _stepSize;
        std::map<ProtoNNParameterIndex, double> _sparsity;
        std::map<ProtoNNParameterIndex, bool> _recomputeWX;

        size_t _iteration = 0;

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
        ProtoNNModelParameter(size_t dimension1, size_t dimension2);

        /// <summary> Get the underlying data matrix. </summary>
        ///
        /// <returns> The underlying data matrix. </returns>
        math::ColumnMatrix<double>& GetData() { return _data; }

        /// <summary> Get the underlying data matrix. </summary>
        ///
        /// <returns> The underlying data matrix. </returns>
        const math::ColumnMatrix<double>& GetData() const { return _data; }

        /// Specifies the interface for gradient computation.
        virtual math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossFunction lossType) = 0;

        /// Specifies the interface for gradient computation.
        virtual math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossFunction lossType) = 0;

    private:
        // The underlying Parameter matrix
        math::ColumnMatrix<double> _data;
    };

    class Param_W : public ProtoNNModelParameter
    {
    public:
        /// <summary></summary>
        Param_W(size_t dimension1, size_t dimension2);

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossFunction lossType) override;

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossFunction lossType) override;
    };

    class Param_B : public ProtoNNModelParameter
    {
    public:
        /// <summary></summary>
        Param_B(size_t dimension1, size_t dimension2);

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossFunction lossType) override;

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossFunction lossType) override;
    };

    class Param_Z : public ProtoNNModelParameter
    {
    public:
        /// <summary></summary>
        Param_Z(size_t dimension1, size_t dimension2);

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, size_t begin, size_t end, ProtoNNLossFunction lossType) override;

        /// <summary></summary>
        math::ColumnMatrix<double> gradient(ProtoNNModelMap& modelMap, ConstColumnMatrixReference X, ConstColumnMatrixReference Y, ConstColumnMatrixReference WX, ConstColumnMatrixReference D, double gamma, ProtoNNLossFunction lossType) override;
    };

    /// <summary> Makes a ProtoNN trainer. </summary>
    ///
    /// <param name="dim"> The dimension. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A ProtoNN trainer </returns>
    std::unique_ptr<trainers::ProtoNNTrainer> MakeProtoNNTrainer(const trainers::ProtoNNTrainerParameters& parameters);
}
}