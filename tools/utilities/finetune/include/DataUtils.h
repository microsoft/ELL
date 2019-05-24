////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataUtils.h (finetune)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

#include <optimization/include/IndexedContainer.h>
#include <optimization/include/MatrixSolution.h>
#include <optimization/include/OptimizationExample.h>
#include <optimization/include/VectorSolution.h>

#include <utilities/include/MemoryLayout.h>

#include <string>
#include <vector>

namespace ell
{
template <typename T>
class VectorContainer : public ell::optimization::IndexedContainer<T>
{
public:
    VectorContainer() = default;
    VectorContainer(std::vector<T> data) :
        _data(std::move(data)) {}

    /// <summary> Returns the number of elements in the container </summary>
    size_t Size() const override { return _data.size(); };

    /// <summary> Checks if the container is empty </summary>
    bool IsEmpty() const { return _data.empty(); };

    /// <summary> Gets the element that corresponds to a given index. </summary>
    T Get(size_t index) const override { return _data[index]; };

    /// <summary> Gets the element that corresponds to a given index. </summary>
    T operator[](size_t index) const { return _data[index]; };

    void Add(T item) { _data.emplace_back(std::move(item)); }

    auto begin() { return _data.begin(); }
    auto begin() const { return _data.cbegin(); }

    auto end() { return _data.end(); }
    auto end() const { return _data.cend(); }

private:
    std::vector<T> _data;
};

// Example types
using UnlabeledExample = ell::math::RowVector<float>;
using BinaryExample = ell::optimization::Example<ell::math::RowVector<float>, double>;
using MultiClassExample = ell::optimization::Example<ell::math::RowVector<float>, int>;
using VectorLabelExample = ell::optimization::Example<ell::math::RowVector<float>, ell::math::RowVector<float>>;

// Solution types
using ScalarLabelSolution = ell::optimization::BiasedVectorSolution<float>;
using VectorLabelSolution = ell::optimization::BiasedMatrixSolution<float>;
using ScalarLabelSolutionExample = typename ScalarLabelSolution::ExampleType;
using VectorLabelSolutionExample = typename VectorLabelSolution::ExampleType;

// DataContainer types
using UnlabeledDataContainer = VectorContainer<UnlabeledExample>;
using BinaryLabelDataContainer = ell::optimization::VectorIndexedContainer<BinaryExample, ScalarLabelSolutionExample>;
using MultiClassDataContainer = VectorContainer<MultiClassExample>;
using VectorLabelDataContainer = ell::optimization::VectorIndexedContainer<VectorLabelExample, VectorLabelSolutionExample>;

/// <summary> Load a dataset with binary labels from a GSDF-format text file. </summary>
BinaryLabelDataContainer LoadBinaryLabelDataContainer(std::string filename);

/// <summary> Load a multiclass dataset from a file, attempting to guess the data format from the filename extension. </summary>
/// The default "-1" value for maxRows means "all rows"
MultiClassDataContainer LoadMultiClassDataContainer(std::string filename, int maxRows = -1);

/// <summary> Load a multiclass dataset from a file with the given format (format strings: "gsdf", "cifar", "mnist"). </summary>
/// The default "-1" value for maxRows means "all rows"
MultiClassDataContainer LoadMultiClassDataContainer(std::string filename, std::string dataFormat, int maxRows = -1);

/// <summary> Load a multiclass dataset from a CIFAR data file. </summary>
/// The default "-1" value for maxRows means "all rows"
MultiClassDataContainer LoadCifarDataContainer(std::string filename, int maxRows = -1);

/// <summary> Load a multiclass dataset from an MNIST data file. </summary>
/// The default "-1" value for maxRows means "all rows"
MultiClassDataContainer LoadMnistDataContainer(std::string filename, int maxRows = -1);

/// <summary> Combine two unlabeled datasets (one containing "features" and one containing "labels") into a labeled dataset. </summary>
VectorLabelDataContainer CreateVectorLabelDataContainer(const UnlabeledDataContainer& features, const UnlabeledDataContainer& labels);

/// <summary> Create a subset of an input dataset, where the inputs and outputs are contiguous blocks of the input dataset's inputs and outputs. </summary>
VectorLabelDataContainer CreateSubBlockVectorLabelDataContainer(const VectorLabelDataContainer& dataset, int inputBlockSize, int outputBlockSize, int index);

/// <summary> Create a subset of an input dataset, where the output is a single element of the source dataset's output vector. </summary>
VectorLabelDataContainer CreateSingleOutputVectorLabelDataContainer(const VectorLabelDataContainer& dataset, int index);

// Split off the inputs/features from a dataset
UnlabeledDataContainer GetDatasetInputs(const BinaryLabelDataContainer& dataset);
UnlabeledDataContainer GetDatasetInputs(const MultiClassDataContainer& dataset);
UnlabeledDataContainer GetDatasetInputs(const VectorLabelDataContainer& dataset);

// Split off the outputs/labels from a dataset
UnlabeledDataContainer GetDatasetOutputs(const VectorLabelDataContainer& dataset);

// Convolution-related functions

/// <summary>
/// Converts a dataset of image data vectors into a dataset where each entry contains the pixel channel values for
/// one (row, column) location in an input image.
/// </summary>
UnlabeledDataContainer GetImageMatrixDataset(const UnlabeledDataContainer& imageData, int numRows, int numColumns, int numChannels);

/// <summary>
/// Converts a dataset of image data vectors into a dataset where each entry contains the image values covered by a convolutional
/// filter located at one (row, column) location in an input image. Commonly known as the "im2col" transform.
///</summary>
UnlabeledDataContainer GetUnrolledImageDataset(const UnlabeledDataContainer& imageData, int numRows, int numColumns, int numChannels, int filterSize, int stride);

/// <summary> Compute the prediction accuracy (fraction of correct classifications) from a labeled dataset and a corresponding dataset of predicted labels. </summary>
double GetModelAccuracy(const BinaryLabelDataContainer& dataset, const UnlabeledDataContainer& predictions);

/// <summary> Compute the prediction accuracy (fraction of correct classifications) from a labeled dataset and a corresponding dataset of predicted labels. </summary>
double GetModelAccuracy(const MultiClassDataContainer& dataset, const UnlabeledDataContainer& predictions);

template <typename T1, typename T2>
ell::math::RowVector<T1> CastVector(const ell::math::ConstRowVectorReference<T2>& v)
{
    auto size = v.Size();
    ell::math::RowVector<T1> result(size);
    for (size_t i = 0; i < size; ++i)
    {
        result[i] = v[i];
    }
    return result;
}
} // namespace ell
