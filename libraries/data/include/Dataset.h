////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Dataset.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"
#include "Dataset.h"
#include "Example.h"
#include "ExampleIterator.h"

// utilities
#include "AbstractInvoker.h"
#include "TypeTraits.h"

// stl
#include <functional>
#include <ostream>
#include <random>
#include <vector>

namespace ell
{
namespace data
{
    // forward declaration of Dataset, since AnyDataset and Dataset have a cyclical dependence
    template <typename ExampleType>
    class Dataset;

    /// <summary> Polymorphic interface for datasets, enables dynamic_cast operations. </summary>
    struct DatasetBase
    {
        virtual ~DatasetBase() = default;
    };

    /// <summary> Implements an untyped data set. This class is used to send data to trainers and evaluators </summary>
    class AnyDataset
    {
    public:
        /// <summary> Constructs an instance of AnyDataset. </summary>
        ///
        /// <param name="pDataset"> Pointer to an DatasetBase. </param>
        /// <param name="fromIndex"> Zero-based index of the first example referenced by the iterator. </param>
        /// <param name="size"> The number of examples referenced by the iterator. </param>
        AnyDataset(const DatasetBase* pDataset, size_t fromIndex, size_t size);

        /// <summary> Gets an example iterator of a given example type. </summary>
        ///
        /// <typeparam name="ExampleType"> Example type. </typeparam>
        ///
        /// <returns> The example iterator. </returns>
        template <typename ExampleType>
        ExampleIterator<ExampleType> GetExampleIterator() const;

        /// <summary> Returns the number of examples in the dataset. </summary>
        ///
        /// <returns> Number of examples. </returns>
        size_t NumExamples() const { return _size; }

    private:
        const DatasetBase* _pDataset;
        size_t _fromIndex;
        size_t _size;
    };

    /// <summary> A data set of a specific example type. </summary>
    template <typename DatasetExampleT>
    class Dataset : public DatasetBase
    {
    public:
        using DatasetExampleType = DatasetExampleT;

        /// <summary> Iterator class. </summary>
        template <typename IteratorExampleType>
        class DatasetExampleIterator : public IExampleIterator<IteratorExampleType>
        {
        public:
            using InternalIteratorType = typename std::vector<DatasetExampleType>::const_iterator;

            /// <summary></summary>
            DatasetExampleIterator(InternalIteratorType begin, InternalIteratorType end);

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if the iterator is currently pointing to a valid iterate. </returns>
            bool IsValid() const override { return _current < _end; }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next() override { ++_current; }

            /// <summary> Gets the current example pointer to by the iterator. </summary>
            ///
            /// <returns> The example. </returns>
            IteratorExampleType Get() const override { return _current->template CopyAs<IteratorExampleType>(); }

        private:
            InternalIteratorType _current;
            InternalIteratorType _end;
        };

        Dataset() = default;

        Dataset(Dataset&&) = default;

        Dataset(const Dataset&) = delete;

        /// <summary> Constructs an instance of Dataset by making shallow copies of supervised examples. </summary>
        ///
        /// <param name="exampleIterator"> The example iterator. </param>
        Dataset(ExampleIterator<DatasetExampleType> exampleIterator);

        /// <summary> Constructs an instance of Dataset from an AnyDataset. </summary>
        ///
        /// <param name="anyDataset"> the AnyDataset. </param>
        Dataset(const AnyDataset& anyDataset);

        Dataset<DatasetExampleType>& operator=(Dataset&&) = default;

        Dataset<DatasetExampleType>& operator=(const Dataset&) = delete;

        /// <summary> Swaps the contents of this dataset with the contents of another. </summary>
        ///
        /// <param name="other"> The other dataset. </param>
        void Swap(Dataset& other);

        /// <summary> Returns the number of examples in the data set. </summary>
        ///
        /// <returns> The number of examples. </returns>
        size_t NumExamples() const { return _examples.size(); }

        /// <summary> Returns the maximal size of any example. </summary>
        ///
        /// <returns> The maximal size of any example. </returns>
        size_t NumFeatures() const { return _numFeatures; }

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        DatasetExampleType& GetExample(size_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const DatasetExampleType& GetExample(size_t index) const;

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        DatasetExampleType& operator[](size_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const DatasetExampleType& operator[](size_t index) const;

        /// <summary> Returns an iterator that traverses the examples. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="size"> The number of examples to iterate over, a value of zero means all
        /// the way to the end. </param>
        ///
        /// <returns> The iterator. </returns>
        template <typename IteratorExampleType = DatasetExampleType>
        ExampleIterator<IteratorExampleType> GetExampleIterator(size_t fromIndex = 0, size_t size = 0) const;

        /// <summary> Gets an example reference iterator. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="size"> The number of examples to iterate over, a value of zero means all
        /// the way to the end. </param>
        ///
        /// <returns> The example reference iterator. </returns>
        ExampleReferenceIterator<DatasetExampleType> GetExampleReferenceIterator(size_t fromIndex = 0, size_t size = 0) const;

        /// <summary> Returns an AnyDataset that represents an interval of examples from this dataset. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example in the AnyDataset. </param>
        /// <param name="size"> The number of examples to include, a value of zero means all
        /// the way to the end. </param>
        ///
        /// <returns> The dataset. </returns>
        AnyDataset GetAnyDataset(size_t fromIndex = 0, size_t size = 0) const { return AnyDataset(this, fromIndex, size); }

        /// <summary> Returns an DataSet whose examples have been converted from this dataset. </summary>
        ///
        /// <typeparam name="otherExampleType"> Example type returned by the transformation function. </typeparam>
        /// <param name="transformationFunction"> The function that is called on each example, returning the transformed example. </param>
        ///
        /// <returns> The dataset. </returns>
        template <typename otherExampleType>
        Dataset<otherExampleType> Transform(std::function<otherExampleType(const DatasetExampleType&)> transformationFunction);

        /// <summary> Adds an example at the bottom of the matrix. </summary>
        ///
        /// <param name="example"> The example. </param>
        void AddExample(DatasetExampleType example);

        /// <summary> Erases all of the examples in the Dataset. </summary>
        void Reset();

        /// <summary> Permutes the rows of the matrix so that a prefix of them is uniformly distributed. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="prefixSize"> Size of the prefix that should be uniformly distributed, zero to permute the entire data set. </param>
        void RandomPermute(std::default_random_engine& rng, size_t prefixSize = 0);

        /// <summary> Randomly permutes a range of rows in the data set so that a prefix of them is uniformly distributed. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="rangeFirstIndex"> Zero-based index of the firest example in the range. </param>
        /// <param name="rangeSize"> Size of the range. </param>
        /// <param name="prefixSize"> Size of the prefix that should be uniformly distributed, zero to permute the entire range. </param>
        void RandomPermute(std::default_random_engine& rng, size_t rangeFirstIndex, size_t rangeSize, size_t prefixSize = 0);

        /// <summary> Choses an example uniformly from a given range and swaps it with a given example (which can either be inside or outside of the range).
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="targetExampleIndex"> Zero-based index of the target example. </param>
        /// <param name="rangeFirstIndex"> Index of the first example in the range from which the example is chosen. </param>
        /// <param name="rangeSize"> Number of examples in the range from which the example is chosen. </param>
        void RandomSwap(std::default_random_engine& rng, size_t targetExampleIndex, size_t rangeFirstIndex, size_t rangeSize);

        /// <summary> Sorts an interval of examples by a certain key. </summary>
        ///
        /// <typeparam name="SortKeyType"> Type of the sort key. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to DatasetExampleType and returns a sort key. </param>
        /// <param name="fromIndex"> Zero-based index of the first row to sort. </param>
        /// <param name="size"> The number of examples to sort. </param>
        template <typename SortKeyType>
        void Sort(SortKeyType sortKey, size_t fromIndex = 0, size_t size = 0);

        /// <summary> Partitions an iterval of examples by a certain Boolean predicate (similar to sorting
        /// by the predicate, but in linear time). </summary>
        ///
        /// <typeparam name="PartitionKeyType"> Type of predicate. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to DatasetExampleType and returns a
        ///  bool. </param>
        /// <param name="fromIndex"> Zero-based index of the first row of the interval. </param>
        /// <param name="size"> The number of examples in the interval. </param>
        template <typename PartitionKeyType>
        void Partition(PartitionKeyType partitionKey, size_t fromIndex = 0, size_t size = 0);

        /// <summary> Prints this object. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        /// <param name="tabs"> The number of tabs. </param>
        /// <param name="fromIndex"> Zero-based index of the first row to print. </param>
        /// <param name="size"> The number of rows to print, or 0 to print until the end. </param>
        void Print(std::ostream& os, size_t tabs = 0, size_t fromIndex = 0, size_t size = 0) const;

    private:
        size_t CorrectRangeSize(size_t fromIndex, size_t size) const;

        std::vector<DatasetExampleType> _examples;
        size_t _numFeatures = 0;
    };

    // friendly names
    typedef Dataset<AutoSupervisedExample> AutoSupervisedDataset;
    typedef Dataset<AutoSupervisedMultiClassExample> AutoSupervisedMultiClassDataset;
    typedef Dataset<DenseSupervisedExample> DenseSupervisedDataset;

    /// <summary> Prints a data set to an ostream. </summary>
    ///
    /// <param name="os"> [in,out] The ostream to write data to. </param>
    /// <param name="dataset"> The dataset. </param>
    ///
    /// <returns> The ostream. </returns>
    template <typename ExampleType>
    std::ostream& operator<<(std::ostream& os, const Dataset<ExampleType>& dataset);

    /// <summary> Helper function that creates a dataset from an example iterator. </summary>
    ///
    /// <typeparam name="ExampleType"> The example type. </typeparam>
    /// <param name="exampleIterator"> The example iterator. </param>
    ///
    /// <returns> A Dataset. </returns>
    template <typename ExampleType>
    Dataset<ExampleType> MakeDataset(ExampleIterator<ExampleType> exampleIterator);
}
}

#include "../tcc/Dataset.tcc"
