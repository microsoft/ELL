# Trainers

The trainers library contains implementations of various machine learning algorithms. Since algorithm names can be very long, we may deviate from our standard naming convention and use acronyms in file and class names. This file lists the algorithms and the acronyms used to name them.

## Linear Trainers
Linear trainers output biased linear predictors by minimizing a regularized empirical loss function. The regularization term may differ from one algorithm to another, and the amount or regularization is controlled by a user-defined parameter. The empirical loss function is typically an arbitrary convex function, which is provided to the algorithm via a template parameter. Linear trainers can solve classification and regression problems (the choice of problem is determined by the choice of the loss function).

* `SGDLinearTrainer`: Implements the "Stochastic Gradient Descent" algorithm. Finds the biased linear predictor that minimzes an L2-regularized empirical loss. The loss function can be any subdifferentiable function.
* `SDSGDLinearTrainer`: Implements the ["Sparse Data Stochastic Gradient Descent"](https://arxiv.org/abs/1612.09147) algorithm, which is mathematically equivalent to SGD but may differ numerically, and uses only sparse vector operations. Therefore, this algorithm should be significantly faster than SGD on sparse datasets, and up to twice as slow on dense datasets.
* `SDCSGDLinearTrainer`: Implements the ["Sparse Data Centered Stochastic Gradient Descent"](https://arxiv.org/abs/1612.09147) algorithm, which is equivalent to centering the training data (shifting its mean to the origin), running SGD, and then correcting the trained predictor so that it can be applied directly to uncentered data. Like SDSGD, this implementation relies on sparse vector operations (where sparsity is with respect to the original uncentered data).

## Decision Forest Trainers
* `SortingForestTrainer`
* `HistogramForestTrainer`

## Utility Trainers
Utility trainers wrap other training algorithms and add some auxilliary functionality to them. 
* `EvaluatingIncrementalTrainer`: Performs an evaluation after each training epoch
* `MultiEpochIncrementalTrainer`: Performs multiple epochs of another trainer
* `SweepingIncrementalTrainer`: Performs a parameter sweep
