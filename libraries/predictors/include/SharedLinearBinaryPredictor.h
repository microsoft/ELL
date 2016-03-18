////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SharedLinearBinaryPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// linear
#include "DoubleVector.h"

// stl
#include <memory>

namespace predictors
{

    /// <summary> A shared linear binary predictor. </summary>
    class SharedLinearBinaryPredictor
    {
    public:

        /// <summary> Constructs an instance of SharedLinearBinaryPredictor. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        SharedLinearBinaryPredictor(uint64 dim);

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
        /// <typeparam name="DataVectorType"> Type of the data vector type. </typeparam>
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> A double. </returns>
        template<typename DataVectorType>
        double Predict(const DataVectorType& example) const;

        /// <summary> Adds the predictor to a stack. </summary>
        ///
        /// <param name="map"> [in,out] The stack. </param>
        void AddToStack(layers::Stack& stack, const layers::CoordinateList& inputCoordinates) const;

    private:
        struct BiasedVector
        {
            BiasedVector(uint64 dim);
            linear::DoubleVector w;
            double b;
        };

        std::shared_ptr<BiasedVector> _sp_predictor;
    };
}

#include "../tcc/SharedLinearBinaryPredictor.tcc"
