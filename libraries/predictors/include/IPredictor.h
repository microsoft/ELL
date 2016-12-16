////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace predictors
{
    /// <summary> Base class for predictors. </summary>
    ///
    /// <typeparam name="T"> The type of the predictions made by this predictor. </typeparam>
    template <typename T>
    class IPredictor
    {
    public:
        /// <summary> The prediction type. </summary>
        using PredictionType = T;
    };
}
}
