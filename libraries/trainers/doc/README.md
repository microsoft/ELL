# Trainers

The trainers library contains implementations of various machine learning algorithms. Since algorithm names can be very long, we may deviate from our standard naming convention and use acronyms in file and class names. This file lists the algorithms and the acronyms used to name them.

## Linear Trainers
Linear trainers output biased linear predictors by minimizing a regularized empirical loss function. The regularization term may differ from one algorithm to another, and the amount or regularization is controlled by a user-defined parameter. The empirical loss function is typically an arbitrary convex function, which is provided to the algorithm via a template parameter. Linear trainers can solve classification and regression problems (the choice of problem is determined by the choice of the loss function).

* `SGDTrainer`: Implements the "Stochastic Gradient Descent" algorithm. Finds the biased linear predictor that minimzes an L2-regularized empirical loss. The loss function can be any subdifferentiable function.
* `SparseDataSGDTrainer`: Implements the ["Sparse Data Stochastic Gradient Descent"](https://arxiv.org/abs/1612.09147) algorithm, which is mathematically equivalent to SGD but may differ numerically, and uses only sparse vector operations. Therefore, this algorithm should be significantly faster than SGD on sparse datasets, and up to twice as slow on dense datasets.
* `SparseDataCenteredSGDTrainer`: Implements the ["Sparse Data Centered Stochastic Gradient Descent"](https://arxiv.org/abs/1612.09147) algorithm, which is equivalent to centering the training data (shifting its mean to the origin), running SGD, and then correcting the trained predictor so that it can be applied directly to uncentered data. Like SparseDataSGD, this implementation relies on sparse vector operations (where sparsity is with respect to the original uncentered data).
* `SDCATrainer`: Implements the "Stochastic Dual Coordinate Ascent" algorithm. The loss function can be any smooth convex function that implement the `Conjugate` and `ConjugateProx` functions. The regularizer can be any smooth convex function that implements `Conjugate` and `ConjugateGradient`.

## Decision Forest Trainers
* `SortingForestTrainer`: A decision forest trainer that sorts the training data by each feature when determining the optimal split. This trainer is only suitable for small datasets. 
* `HistogramForestTrainer`: A decision forest trainer that doesn't sort the training data, and instead finds the optimal split using a histogram of each feature. 

## Data Statistics Calculators
These simple algorithms have the same API as trainers and calculate simple statistics from the dataset.
* `MeanCalculator`: Applies an arbitrary transformation to each coordinate (e.g., absolute value) and computes the mean of the transformed data vectors in the dataset. 

## Utility Trainers
Utility trainers wrap other training algorithms and add some auxilliary functionality to them.
* `EvaluatingTrainer`: Performs an evaluation after each training epoch
* `SweepingTrainer`: Performs a parameter sweep
