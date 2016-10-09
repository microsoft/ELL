////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Dataset.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Example.h"
#include "DataVector.h"
#include "Dataset.h"
#include "ExampleIterator.h"

// utilities
#include "StlIterator.h"
#include "AbstractInvoker.h"
#include "TypeTraits.h"

// stl
#include <ostream>
#include <random>
#include <vector>
#include <functional>

namespace emll
{
namespace data
{
    /// <summary> A functor class that calls the GetExampleIterator member of a Dataset. </summary>
    ///
    /// <typeparam name="IteratorExampleType"> Example type. </typeparam>
    template<typename IteratorExampleType>
    class GetExampleIteratorFunctor
    {
    public:
        /// <summary> Functor return type. </summary>
        using ReturnType = ExampleIterator<IteratorExampleType>;

        /// <summary> Constructor. </summary>
        ///
        /// <param name="fromIndex"> Zero-based index of the first example referenced by the iterator. </param>
        /// <param name="size"> The number of examples referenced by the iterator. </param>
        GetExampleIteratorFunctor(size_t fromIndex, size_t size); // TODO rename to GetExampleI...

        /// <summary> Function call operator. Calls a dataset's GetExampleIterator member. </summary>
        ///
        /// <typeparam name="DatasetType"> Dataset type. </typeparam>
        /// <param name="dataset"> The dataset. </param>
        ///
        /// <returns> The example iterator returned by the call to GetExampleIterator. </returns>
        template<typename DatasetType>
        ReturnType operator()(const DatasetType& dataset) const;
    
    private:
        size_t _fromIndex;
        size_t _size;
    };

    /// <summary> Polymorphic interface for datasets, enables dynamic_cast operations. </summary>
    struct IDataset 
    {
        virtual ~IDataset() = default;
    };

    /// <summary> Implements an untyped data set. This class is used to send data to trainers and evaluators </summary>
    class AnyDataset
    {
    public:

        /// <summary> Constructs an instance of AnyDataset. </summary>
        ///
        /// <param name="pDataset"> Pointer to an IDataset. </param>
        /// <param name="fromIndex"> Zero-based index of the first example referenced by the iterator. </param>
        /// <param name="size"> The number of examples referenced by the iterator. </param>
        AnyDataset(const IDataset* pDataset, size_t fromIndex, size_t size);

        /// <summary> Gets an example iterator of a given example type. </summary>
        ///
        /// <typeparam name="ExampleType"> Example type. </typeparam>
        ///
        /// <returns> The example iterator. </returns>
        template<typename ExampleType>
        ExampleIterator<ExampleType> GetExampleIterator(); // TODO rename to GetExampleIterator

        /// <summary> Returns the number of examples in the dataset. </summary>
        ///
        /// <returns> Number of examples. </returns>
        size_t NumExamples() const { return _size; }

    private:
        const IDataset* _pDataset;
        size_t _fromIndex;
        size_t _size;
    };

    /// <summary> A data set of a specific example type. </summary>
    template <typename DatasetExampleType>
    class Dataset : public IDataset
    {
    public:
        /// <summary> Iterator class. </summary>
        template<typename IteratorExampleType> 
        class DatasetExampleIterator : public IExampleIterator<IteratorExampleType>
        {
        public:
            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            virtual bool IsValid() const override { return _current < _end; }

            /// <summary> Proceeds to the Next iterate. </summary>
            virtual void Next() override { ++_current; }

            /// <summary> Gets the current example pointer to by the iterator. </summary>
            ///
            /// <returns> The example. </returns>
            virtual IteratorExampleType Get() const override { return _current->ToExample<IteratorExampleType>(); } 

            // TODO: move this to private and make the dataset a friend
            using InternalIteratorType = typename std::vector<DatasetExampleType>::const_iterator;
            DatasetExampleIterator(InternalIteratorType begin, InternalIteratorType end);

        private:
            InternalIteratorType _current;
            InternalIteratorType _end;
        };

        Dataset() = default;

        Dataset(Dataset&&) = default;

        explicit Dataset(const Dataset&) = default; // TODO why is this allowed?

        /// <summary> Constructs an instance of Dataset by making shallow copies of supervised examples. </summary>
        ///
        /// <param name="exampleIterator"> The example iterator. </param>
        Dataset(ExampleIterator<DatasetExampleType> exampleIterator);

        Dataset<DatasetExampleType>& operator=(Dataset&&) = default;

        Dataset<DatasetExampleType>& operator=(const Dataset&) = delete;

        /// <summary> Returns the number of examples in the data set. </summary>
        ///
        /// <returns> The number of examples. </returns>
        size_t NumExamples() const { return _examples.size(); }

        /// <summary> Returns the maximal size of any example. </summary>
        ///
        /// <returns> The maximal size of any example. </returns>
        size_t GetMaxDataVectorSize() const { return _maxExampleSize; }

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
        template<typename IteratorExampleType = DatasetExampleType>
        ExampleIterator<IteratorExampleType> GetExampleIterator(size_t fromIndex = 0, size_t size = 0) const;

        /// <summary> Returns an AnyDataset that represents an interval of examples from this dataset. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example in the AnyDataset. </param>
        /// <param name="size"> The number of examples to include, a value of zero means all
        /// the way to the end. </param>
        ///
        /// <returns> The iterator. </returns>
        AnyDataset GetAnyDataset(size_t fromIndex = 0, size_t size = 0) const { return AnyDataset(this, fromIndex, size); }

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
        size_t _maxExampleSize = 0;
    };

    /// <summary> Helper function that makes a data set out of an example iterator. </summary>
    ///
    /// <typeparam name="ExampleType"> Example type. </typeparam>
    /// <param name="iterator"> The example iterator. </param>
    ///
    /// <returns> A Dataset. </returns>
    template<typename ExampleType>
    Dataset<ExampleType> MakeDataset(ExampleIterator<ExampleType> iterator);

    // friendly name
    typedef Dataset<AutoSupervisedExample> AutoSupervisedDataset;

    /// <summary> Prints a data set to an ostream. </summary>
    ///
    /// <param name="os"> [in,out] The ostream to write data to. </param>
    /// <param name="dataset"> The dataset. </param>
    ///
    /// <returns> The ostream. </returns>
    template <typename ExampleType>
    std::ostream& operator<<(std::ostream& os, const Dataset<ExampleType>& dataset);
}
}

#include "../tcc/Dataset.tcc"
