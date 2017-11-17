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
    ///
    /// <typeparam name="ElementType"> The fundamental type used by this predictor. </typeparam>
    template <typename ElementType>
    class LinearPredictor : public IPredictor<ElementType>, public utilities::IArchivable
    {
    public:
        /// <summary> Type of the data vector expected by this predictor type. </summary>
        using DataVectorType = data::AutoDataVector;

        /// <summary> Default Constructor. </summary>
        LinearPredictor() = default;

        /// <summary> Constructs an instance of LinearPredictor. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        LinearPredictor(size_t dim);

        /// <summary> Constructs an instance of LinearPredictor. </summary>
        ///
        /// <param name="weights"> The weights. </param>
        /// <param name="bias"> The bias. </param>
        LinearPredictor(const math::ColumnVector<ElementType>& weights, ElementType bias);

        /// <summary> Constructs an instance of a LinearPredictor from an existing one. </summary>
        ///
        /// <param name="dim"> The dimension. </param>
        /// <typeparam name="OtherElementType"> The fundamental type used by the other predictor.
        /// This allows a linear predictor to be initialized from another whose fundamental type is different,
        /// but compatible. Since trainers always output a linear predictor of type double, this can be used
        /// to create the same predictor using float.
        /// </typeparam>
        template <typename OtherElementType>
        LinearPredictor(const LinearPredictor<OtherElementType>& other);

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        math::ColumnVector<ElementType>& GetWeights() { return _w; }

        /// <summary> Returns the underlying DoubleVector. </summary>
        ///
        /// <returns> The underlying vector. </returns>
        const math::ConstColumnVectorReference<ElementType>& GetWeights() const { return _w; }

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        ElementType& GetBias() { return _b; }

        /// <summary> Returns the underlying bias. </summary>
        ///
        /// <returns> The bias. </returns>
        ElementType GetBias() const { return _b; }

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
        ElementType Predict(const DataVectorType& dataVector) const;

        /// <summary> Returns a vector of dataVector elements weighted by the predictor weights. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> The weighted elements vector. </returns>
        DataVectorType GetWeightedElements(const DataVectorType& dataVector) const;

        /// <summary> Scales the linear predictor by a scalar </summary>
        ///
        /// <param name="scalar"> The scalar. </param>
        void Scale(ElementType scalar);

        /// <summary> Resets the linear predictor to the zero vector with zero bias. </summary>
        void Reset();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("LinearPredictor"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        math::ColumnVector<ElementType> _w;
        ElementType _b = 0;
    };
}
}

#include "../tcc/LinearPredictor.tcc"
