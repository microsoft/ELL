////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ILayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IPredictor.h"

// math
#include "Vector.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <memory>

namespace ell
{
namespace predictors
{
namespace neural
{

/// <summary> A layer in a neural network. </summary>
class ILayer
{
public:
    /// <summary> The input and output vector type for layers. </summary>
    using LayerVector = math::ColumnVector<double>;

    /// <summary> The matrix type for matrix operations inside certain layers. </summary>
    using LayerMatrix = math::RowMatrix<double>;

    /// <summary> The matrix type for matrix operations inside certain layers. </summary>
    using LayerReferenceMatrix = math::MatrixReference<double, math::MatrixLayout::rowMajor>;

    virtual ~ILayer() = default;

    /// <summary> Feeds the input forward throught the layer and returns a reference to the output. </summary>
    ///
    /// <param name="input"> The input vector. </param>
    ///
    /// <returns> A reference to the output vector. </returns>
    virtual LayerVector& FeedForward(const LayerVector& input) = 0;

    /// <summary> Returns a reference to the output values, which is the result after the last #Forward call. </summary>
    ///
    /// <returns> A reference to the output vector. </returns>
    virtual LayerVector& GetOutput() = 0;

    /// <summary> Returns the expected size of the input vector. </summary>
    ///
    /// <returns> Expected size of the input vector. </returns>
    virtual size_t NumInputs() const = 0;

    /// <summary> Returns the size of the output vector. </summary>
    ///
    /// <returns> Size of the output vector. </returns>
    virtual size_t NumOutputs() const = 0;

    /// <summary> Adds an object's properties to an `Archiver` </summary>
    ///
    /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
    virtual void WriteToArchive(utilities::Archiver& archiver) const = 0;

    /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
    ///
    /// <param name="archiver"> The `Archiver` to get state from </param>
    virtual void ReadFromArchive(utilities::Unarchiver& archiver) = 0;
};

}
}
}

