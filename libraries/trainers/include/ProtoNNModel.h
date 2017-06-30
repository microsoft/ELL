////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainer.h (trainers)
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
    enum ProtoNNLossType { L2, L4 };

    /// <summary> Parameters for the ProtoNN trainer. </summary>
    struct ProtoNNTrainerParameters
    {
        size_t projectedDimesion;
		size_t numPrototypesPerLabel;
        size_t numPrototypes;
        size_t numLabels;
        double lambdaW;
        double lambdaZ;
        double lambdaB;
        double gamma;
        ProtoNNLossType lossType;
        size_t numIters;
        size_t numInnerIters;
        bool verbose;
    };

    enum ProtoNNParameterIndex { W = 0, B, Z };

    typedef math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> ConstColumnMatrixReference;

	namespace ProtoNN
	{
		static const double ArmijoStepTolerance = 0.02;

		static const double DefaultStepSize = 0.2;
	}
}
}

