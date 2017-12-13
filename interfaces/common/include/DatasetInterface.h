////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DatasetInterface.h (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <memory>
#include <vector>

namespace ELL_API
{

/// <summary> AutoDataVector represents a vector of doubles that may be stored either as a dense vector or
/// a sparse vector depending on how the dataset was stored. </summary>
class AutoDataVector
{
public:
    AutoDataVector();
    AutoDataVector(const std::vector<double>& data);

    /// <summary> Returns the data as a dense std::vector of doubles. </summary>
    /// <returns> The std::vector of doubles. </returns>
    std::vector<double> ToArray() const;

    // These methods are about 10 times faster than ell.DoubleVector(data.ToArray())
    // because of the way that python iterates vectors (wrapping every value in a PYOBJECT).

    /// <summary> Copy the data in this vector to the given buffer, resizing the buffer if necessary </summary>
    /// <param name="buffer"> The buffer to copy the data into. </param>
    void CopyTo(std::vector<double>& buffer);

    /// <summary> Copy the data in this vector to the given float buffer, resizing the buffer if necessary. </summary>
    /// <param name="buffer"> The buffer to copy the data into. </param>
    void CopyTo(std::vector<float>& buffer);
private:
    class AutoDataVectorImpl;
    friend class AutoSupervisedExample;
    friend class ProtoNNPredictor;
    friend class Map;
    std::shared_ptr<AutoDataVectorImpl> _impl;
};

/// <summary> AutoSupervisedExample class represents training or testing data, stored as an AutoDataVector, combined with the expected label. </summary>
class AutoSupervisedExample
{
public:
    AutoSupervisedExample();

    /// <summary> Returns the expected label fro this example data. </summary>
    /// <returns> The expected label. </returns>
    double GetLabel() const;

    /// <summary> Returns the example data to be used in training or testing. </summary>
    /// <returns> The example data. </returns>
    AutoDataVector GetData() const;
private:
    class AutoSupervisedExampleImpl;
    friend class AutoSupervisedDataset;
    std::shared_ptr<AutoSupervisedExampleImpl> _impl;
};

/// <summary> AutoSupervisedDataset manages a supervised dataset which is a dataset consisting of AutoSupervisedExamples.
/// An AutoSupervisedDataset can load both sparse and dense file formats. </summary>
class AutoSupervisedDataset
{
public:
    AutoSupervisedDataset();

    /// <summary> Returns the number of examples in the data set. </summary>
    /// <returns> The number of examples. </returns>
    size_t NumExamples() const;

    /// <summary> Returns the maximal size of any example. </summary>
    /// <returns> The maximal size of any example. </returns>
    size_t NumFeatures() const;

    /// <summary> Returns a specified example. </summary>
    /// <param name="index"> Zero-based index of the row. </param>
    AutoSupervisedExample GetExample(size_t index);

    void Load(std::string filename);
    void Save(std::string filename);

private:
    class AutoSupervisedDatasetImpl;
    friend class ProtoNNTrainer;
    std::shared_ptr<AutoSupervisedDatasetImpl> _impl;
};

} // end namespace
