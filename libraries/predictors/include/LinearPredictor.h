////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// layers
#include "Model.h"
#include "Coordinate.h"

// linear
#include "DoubleVector.h"

// datasets
#include "IDataVector.h"

// stl
#include <cstdint>
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
        LinearPredictor(uint64_t dim);

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        linear::DoubleVector& GetVector() { return _w; }

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        const linear::DoubleVector& GetVector() const { return _w; }

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        double& GetBias() { return _b; }

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        double GetBias() const { return _b; }

        /// <summary> Gets the dimension of the linear predictor. </summary>
        ///
        /// <returns> The dimension. </returns>
        uint64_t GetDimension() const;

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> A double. </returns>
        double Predict(const dataset::IDataVector& dataVector) const;

        /// <summary> Scales the linear predictor by a scalar </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void Scale(double scalar);

        /// <summary> Resets the linear predictor to the zero vector with zero bias. </summary>
        void Reset();

        /// <summary> Adds the predictor to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        void AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const;

        /// <summary> Swaps two linear predictors </summary>
        ///
        /// <param name="u"> [in,out] A LinearPredictor. </param>
        /// <param name="v"> [in,out] A LinearPredictor. </param>
        static void Swap(LinearPredictor& u, LinearPredictor& v);

    private:
        linear::DoubleVector _w;
        double _b;
    };
}
