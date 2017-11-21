////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNModel.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <memory>
#include <map>

#include "Matrix.h"

namespace ell
{
namespace trainers
{
#if !defined(SWIG)
    enum class ProtoNNLossFunction { L2, L4 };
#endif

    /// <summary> Parameters for the ProtoNN trainer. </summary>
    struct ProtoNNTrainerParameters
    {
        ///<summary>The number of features</summary>
        size_t numFeatures;

        ///<summary>The number of labels</summary>
        size_t numLabels;

        ///<summary>The projected dimension</summary>
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
#if defined(SWIG)
        int lossFunction;
#else
        ProtoNNLossFunction lossFunction;
#endif

        ///<summary>The number of iterations for training</summary>
        size_t numIterations;

        ///<summary>The number of inner iterations for training</summary>
        size_t numInnerIterations;

        ///<summary>Whether to output diagnostic information to std::cout.</summary>
        bool verbose;
    };

}
}

