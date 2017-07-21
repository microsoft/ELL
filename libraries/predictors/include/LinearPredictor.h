////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// math
#include "Vector.h"

// datasets
#include "AutoDataVector.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <memory>

namespace ell
{
namespace predictors
{
    /// <summary> A linear binary predictor. </summary>
    class LinearPredictor : public IPredictor<double>, public utilities::IArchivable
    {
    public:
        /// <summary> Type of the data vector expected by this predictor type. </summary>
        using DataVectorType = data::AutoDataVector;

        /// <summary> Default Constructor. </summary>
        LinearPredictor();

        /// <summary> Constructs an instance of LinearPredictor. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        LinearPredictor(size_t dim);

        /// <summary> Constructs an instance of LinearPredictor. </summary>
        ///
        /// <param name="weights"> The weights. </param>
        /// <param name="bias"> The bias. </param>
        LinearPredictor(const math::ColumnVector<double>& weights, double bias);

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        math::ColumnVector<double>& GetWeights() { return _w; }

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        const math::ColumnConstVectorReference<double>& GetWeights() const { return _w; }

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
        size_t Size() const { return _w.Size(); }

        /// <summary> Resizes the weight vector to a given size. </summary>
        ///
        /// <param name="size"> The new size. </param>
        void Resize(size_t size);

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> The prediction. </returns>
        double Predict(const DataVectorType& dataVector) const;

        /// <summary> Returns a vector of dataVector elements weighted by the predictor weights. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> The weighted elements vector. </returns>
        DataVectorType GetWeightedElements(const DataVectorType& dataVector) const;

        /// <summary> Scales the linear predictor by a scalar </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void Scale(double scalar);

        /// <summary> Resets the linear predictor to the zero vector with zero bias. </summary>
        void Reset();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "LinearPredictor"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        math::ColumnVector<double> _w;
        double _b;
    };
}
}
