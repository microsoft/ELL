////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TrainerInterface.h (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <memory>
#include "ModelInterface.h"
#include "DatasetInterface.h"

namespace ELL_API
{

/// <summary> The loss function to use durng ProtoNN training. </summary>
// Note: we are using this weird struct because SWIG doesn't import class enum properly.  
// It generates ell.ProtoNNLossFunction_L2 and ell.ProtoNNLossFunction_L4, whereas we
// want ell.ProtoNNLossFunction.L2 and ell.ProtoNNLossFunction.L2.
struct ProtoNNLossFunction
{
public:
    enum { 
        ///<summary>The diff squared</summary>
        L2,
        ///<summary>The diff to the power of 4</summary>
        L4 
    };
};

/// <summary> Parameters for the ProtoNN trainer. </summary>
struct ProtoNNTrainerParameters
{
    ///<summary>The number of features</summary>
    size_t numFeatures;

    ///<summary>The number of labels</summary>
    size_t numLabels;

    ///<summary>The projected dimension (0 means auto)</summary>
    size_t projectedDimension;

    ///<summary>The number of prototypes per label</summary>
    size_t numPrototypesPerLabel;

    ///<summary>The sparsity parameter for W</summary>
    double sparsityW;

    ///<summary>The sparsity parameter for Z</summary>
    double sparsityZ;

    ///<summary>The sparsity parameter for B</summary>
    double sparsityB;

    ///<summary>The gamma value</summary>
    double gamma;

    ///<summary>The choice of loss function</summary>
    int lossFunction;

    ///<summary>The number of iterations for training</summary>
    size_t numIterations = 20;

    ///<summary>The number of prototypes per label</summary>
    size_t numInnerIterations = 1;

    ///<summary>Whether to output diagnostic messages during the training process</summary>
    bool verbose = false;
};

class ProtoNNPredictor
{
public:
    ProtoNNPredictor();

    /// <summary> Run the trained predictor over the given input data </summary>
    /// <param name="data"> The input data. </param>
    std::vector<double> Predict(const AutoDataVector& data) const;

    /// <summary> Run the trained predictor over the given input data </summary>
    /// <param name="data"> The input data. </param>
    std::vector<double> Predict(const std::vector<double>& data) const;

    ///<summary> Create an ELL model containing the ProtoNN predictor </summary>
    Map GetMap() const;
private:
    class ProtoNNPredictorImpl;
    std::shared_ptr<ProtoNNPredictorImpl> _impl;
    friend class ProtoNNTrainer;
};

class ProtoNNTrainer
{
public:
    /// <summary> Construct a new ProtoNNTrainer. </summary>
    /// <param name="parameters"> The training parameters. </param>
    ProtoNNTrainer(const ProtoNNTrainerParameters& parameters);

    /// <summary> Provide the training dataset. </summary>
    /// <param name="dataset"> The training data where each row is numFeatures wide, as defined in the parameters. </param>
    void SetDataset(const AutoSupervisedDataset& dataset);

    /// <summary> Trains a ProtoNN predictor for the dataset provided in SetDataset. 
    /// Currently you should only call Update once.</summary>
    void Update();

    /// <summary> Returns the current ProtoNN predictor. </summary>
    /// <returns> The current predictor. </returns>
    ProtoNNPredictor GetPredictor() const;

private:
    class ProtoNNTrainerImpl;
    std::shared_ptr<ProtoNNTrainerImpl> _impl;
};

} // end namespace
