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

// utilities
#include "ISerializable.h"
#include "Serializer.h"

// stl
#include <cstdint>
#include <memory>

namespace predictors
{
    /// <summary> A linear binary predictor. </summary>
    class LinearPredictor : public utilities::ISerializable
    {
    public:
        /// <summary> Default Constructor. </summary>
        LinearPredictor();

        /// <summary> Constructs an instance of Linear. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        LinearPredictor(uint64_t dim);

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        linear::DoubleVector& GetWeights() { return _w; }

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        const linear::DoubleVector& GetWeights() const { return _w; }

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
        uint64_t GetDimension() const { return _w.Size(); }

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
        ///
        /// <returns> The predictor's output coordinates in the model. </returns>
        layers::CoordinateList AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Node"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

    private:
        linear::DoubleVector _w;
        double _b;
    };
}
