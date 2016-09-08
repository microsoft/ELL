////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDataset.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Example.h"
#include "IDataVector.h"
#include "RowDataset.h"

// utilities
#include "StlIterator.h"

// stl
#include <ostream>
#include <random>
#include <vector>

namespace dataset
{
    /// <summary> A row-major dataset of examples. </summary>
    template <typename ExampleType>
    class RowDataset
    {
    public:
        typedef utilities::VectorIterator<ExampleType> Iterator;

        RowDataset() = default;

        RowDataset(RowDataset&&) = default;

        explicit RowDataset(const RowDataset&) = default;

        /// <summary> Constructs an instance of RowDataset by making shallow copies of supervised examples. </summary>
        ///
        /// <param name="exampleIterator"> The example iterator. </param>
        RowDataset(Iterator exampleIterator);

        RowDataset<ExampleType>& operator=(RowDataset&&) = default;

        RowDataset<ExampleType>& operator=(const RowDataset&) = delete;

        /// <summary> Returns the number of examples in the dataset. </summary>
        ///
        /// <returns> The number of examples. </returns>
        uint64_t NumExamples() const { return _examples.size(); }

        /// <summary> Returns the maximal size of any example. </summary>
        ///
        /// <returns> The maximal size of any example. </returns>
        uint64_t GetMaxDataVectorSize() const { return _maxExampleSize; }

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        ExampleType& GetExample(uint64_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const ExampleType& GetExample(uint64_t index) const;

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        ExampleType& operator[](uint64_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const ExampleType& operator[](uint64_t index) const;

        /// <summary> Returns an iterator that traverses the examples. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="numExamples"> The number of examples to iterate over, a value of zero means all
        /// examples. </param>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator(uint64_t fromRowIndex = 0, uint64_t size = 0) const;

        /// <summary> Adds an example at the bottom of the matrix. </summary>
        ///
        /// <param name="example"> The example. </param>
        void AddExample(ExampleType example);

        /// <summary> Erases all of the examples in the RowDataset. </summary>
        void Reset();

        /// <summary> Permutes the rows of the matrix so that a prefix of them is uniformly distributed. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="prefixSize"> Size of the prefix that should be uniformly distributed, zero to permute the entire dataset. </param>
        void RandomPermute(std::default_random_engine& rng, size_t prefixSize = 0);

        /// <summary> Randomly permutes a range of rows in the dataset so that a prefix of them is uniformly distributed. </summary>
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
        void RandomSwap(std::default_random_engine& rng, uint64_t targetExampleIndex, uint64_t rangeFirstIndex, uint64_t rangeSize);

        /// <summary> Sorts an interval of examples by a certain key. </summary>
        ///
        /// <typeparam name="SortKeyType"> Type of the sort key. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to ExampleType and returns a sort key. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row to sort. </param>
        /// <param name="size"> The number of examples to sort. </param>
        template <typename SortKeyType>
        void Sort(SortKeyType sortKey, uint64_t fromRowIndex = 0, uint64_t size = 0);

        /// <summary> Partitions an iterval of examples by a certain Boolean predicate (similar to sorting
        /// by the predicate, but in linear time). </summary>
        ///
        /// <typeparam name="PartitionKeyType"> Type of predicate. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to ExampleType and returns a
        ///  bool. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row of the interval. </param>
        /// <param name="size"> The number of examples in the interval. </param>
        template <typename PartitionKeyType>
        void Partition(PartitionKeyType partitionKey, uint64_t fromRowIndex = 0, uint64_t size = 0);

        /// <summary> Prints this object. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        /// <param name="tabs"> The number of tabs. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row to print. </param>
        /// <param name="size"> The number of rows to print, or 0 to print until the end. </param>
        void Print(std::ostream& os, size_t tabs = 0, uint64_t fromRowIndex = 0, uint64_t size = 0) const;

    private:
        uint64_t CorrectRangeSize(uint64_t fromRowIndex, uint64_t size) const;

        std::vector<ExampleType> _examples;
        uint64_t _maxExampleSize = 0;
    };

    typedef RowDataset<GenericSupervisedExample> GenericRowDataset;

    template <typename ExampleType>
    std::ostream& operator<<(std::ostream& os, RowDataset<ExampleType>& dataset);
}

#include "../tcc/RowDataset.tcc"
