////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Linear.h (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// layers
#include "Stack.h"
#include "Coordinate.h"

// linear
#include "DoubleVector.h"

// datasets
#include "IDataVector.h"

// stl
#include <memory>

namespace predictors
{
    /// <summary> A linear binary predictor. </summary>
    class LinearPredictor
    {
    public:

        /// <summary> Constructs an instance of Linear. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        LinearPredictor(uint64 dim);

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        linear::DoubleVector& GetVector();

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        const linear::DoubleVector& GetVector() const;

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        double& GetBias();

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        double GetBias() const;

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> A double. </returns>
        double Predict(const dataset::IDataVector& dataVector) const;

        /// <summary> Adds the predictor to a map. </summary>
        ///
        /// <param name="map"> [in,out] The map. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        void AddToStack(layers::Stack& stack, const layers::CoordinateList& inputCoordinates) const;

    private:
        linear::DoubleVector _w;
        double _b;
    };
}
