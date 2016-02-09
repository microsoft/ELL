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

#include "types.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// stl
#include <memory>

// linear
#include "DoubleVector.h"

namespace predictors
{

    class SharedLinearBinaryPredictor
    {
    public:
        SharedLinearBinaryPredictor(uint64 dim);

        /// \returns the underlying DoubleVector
        ///
        linear::DoubleVector& GetVector();

        /// \returns the const reference underlying DoubleVector
        ///
        const linear::DoubleVector& GetVector() const;

        /// \returns the underlying bias
        ///
        double& GetBias();

        /// \returns the underlying bias
        ///
        double GetBias() const;

        /// \returns the output of the predictor for a given example
        ///
        template<typename DataVectorType>
        double Predict(const DataVectorType& example) const;

        /// Adds the predictor to a map
        ///
        void AddTo(layers::Map& map, const layers::CoordinateList& inputCoordinates) const;

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
