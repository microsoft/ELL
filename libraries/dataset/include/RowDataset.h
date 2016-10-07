////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDataset.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Example.h"
#include "DataVector.h"
#include "RowDataset.h"

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
namespace dataset
{
    template<typename ExampleType> // TODO document
    struct IExampleIterator
    {
        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if the iterator is currently pointing to a valid iterate. </returns>
        virtual bool IsValid() const = 0;

        /// <summary> Proceeds to the Next iterate. </summary>
        virtual void Next() = 0;

        /// <summary> Returns the current example. </summary>
        ///
        /// <returns> An example. </returns>
        virtual ExampleType Get() const = 0;
    };

    template<typename ExampleType> // TODO document
    class ExampleIterator
    {
    public:
        ExampleIterator(std::shared_ptr<IExampleIterator<ExampleType>>&& iterator) : _iterator(std::move(iterator)) {} // TODO move to tcc, document

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _iterator->IsValid();  }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next() { _iterator->Next(); }

        /// <summary> Returns the current example. </summary>
        ///
        /// <returns> An example. </returns>
        ExampleType Get() const { return _iterator->Get(); }

    private:
        std::shared_ptr<IExampleIterator<ExampleType>> _iterator;
    };

    template<typename ExampleType>
    struct GetIteratorAbstractor
    {
        using ReturnType = ExampleIterator<ExampleType>;

        GetIteratorAbstractor(size_t fromIndex, size_t size) : _fromIndex(fromIndex), _size(size) {}

        template<typename RowDatasetType>
        ReturnType operator()(const RowDatasetType& dataset) const
        {
            return dataset.GetIterator<ExampleType>(_fromIndex, _size);
        }

        size_t _fromIndex;
        size_t _size;
    };

    struct IRowDataset 
    {
        virtual ~IRowDataset() {}
    };

    class DataSet
    {
    public:
        DataSet(const IRowDataset* pDataset, size_t fromIndex, size_t size) : _pDataset(pDataset), _fromIndex(fromIndex), _size(size) {}

        template<typename ExampleType, utilities::IsSame<typename ExampleType::MetadataType, WeightLabel> Concept = true>
        ExampleIterator<ExampleType> GetIterator()
        {
            GetIteratorAbstractor<ExampleType> abstractor(_fromIndex, _size);
            return utilities::AbstractInvoker<IRowDataset, RowDataset<dataset::AutoSupervisedExample>, RowDataset<dataset::DenseSupervisedExample>>::Invoke(abstractor, *_pDataset);
        }

        size_t Size() const { return _size; }

    private:
        const IRowDataset* _pDataset;
        size_t _fromIndex;
        size_t _size;
    };

    /// <summary> A row-major dataset of examples. </summary>
    template <typename DataSetExampleType>
    class RowDataset : public IRowDataset
    {
    public:
        /// <summary> Iterator class. </summary>
        template<typename IteratorExampleType> 
        class DatasetExampleIterator : public IExampleIterator<IteratorExampleType>
        {
        public:
            using IteratorType = typename std::vector<DataSetExampleType>::const_iterator;
            DatasetExampleIterator(IteratorType begin, IteratorType end) : _current(begin), _end(end) {} // TODO move this private, move implementation to tcc file

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            virtual bool IsValid() const override { return _current < _end; }

            /// <summary> Proceeds to the Next iterate. </summary>
            virtual void Next() override { ++_current; }

            /// <summary> Gets the current example pointer to by the iterator. </summary>
            ///
            /// <returns> The example. </returns>
            virtual IteratorExampleType Get() const override { return _current->ToExample<IteratorExampleType::DataVectorType, IteratorExampleType::MetadataType>(); } // TODO change ToExample to take one template  param, which is the example type

        private:
            friend RowDataset<DataSetExampleType>;

            IteratorType _current;
            IteratorType _end;
        };

        RowDataset() = default;

        RowDataset(RowDataset&&) = default;

        explicit RowDataset(const RowDataset&) = default; // TODO why is this allowed?

        /// <summary> Constructs an instance of RowDataset by making shallow copies of supervised examples. </summary>
        ///
        /// <param name="exampleIterator"> The example iterator. </param>
        RowDataset(ExampleIterator<DataSetExampleType> exampleIterator);

        RowDataset<DataSetExampleType>& operator=(RowDataset&&) = default;

        RowDataset<DataSetExampleType>& operator=(const RowDataset&) = delete;

        /// <summary> Returns the number of examples in the dataset. </summary>
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
        DataSetExampleType& GetExample(size_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const DataSetExampleType& GetExample(size_t index) const;

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        DataSetExampleType& operator[](size_t index);

        /// <summary> Returns a const reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Const reference to the specified example. </returns>
        const DataSetExampleType& operator[](size_t index) const;

        /// <summary> Returns an iterator that traverses the examples. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="numExamples"> The number of examples to iterate over, a value of zero means all
        /// examples. </param>
        ///
        /// <returns> The iterator. </returns>
        template<typename IteratorExampleType = DataSetExampleType>
        ExampleIterator<IteratorExampleType> GetIterator(size_t fromRowIndex = 0, size_t size = 0) const;

        DataSet GetDataSet(size_t fromIndex = 0, size_t size = 0) const { return DataSet(this, fromIndex, size); }

        /// <summary> Adds an example at the bottom of the matrix. </summary>
        ///
        /// <param name="example"> The example. </param>
        void AddExample(DataSetExampleType example);

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
        void RandomSwap(std::default_random_engine& rng, size_t targetExampleIndex, size_t rangeFirstIndex, size_t rangeSize);

        /// <summary> Sorts an interval of examples by a certain key. </summary>
        ///
        /// <typeparam name="SortKeyType"> Type of the sort key. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to DataSetExampleType and returns a sort key. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row to sort. </param>
        /// <param name="size"> The number of examples to sort. </param>
        template <typename SortKeyType>
        void Sort(SortKeyType sortKey, size_t fromRowIndex = 0, size_t size = 0);

        /// <summary> Partitions an iterval of examples by a certain Boolean predicate (similar to sorting
        /// by the predicate, but in linear time). </summary>
        ///
        /// <typeparam name="PartitionKeyType"> Type of predicate. </typeparam>
        /// <param name="sortKey"> A function that takes const reference to DataSetExampleType and returns a
        ///  bool. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row of the interval. </param>
        /// <param name="size"> The number of examples in the interval. </param>
        template <typename PartitionKeyType>
        void Partition(PartitionKeyType partitionKey, size_t fromRowIndex = 0, size_t size = 0);

        /// <summary> Prints this object. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        /// <param name="tabs"> The number of tabs. </param>
        /// <param name="fromRowIndex"> Zero-based index of the first row to print. </param>
        /// <param name="size"> The number of rows to print, or 0 to print until the end. </param>
        void Print(std::ostream& os, size_t tabs = 0, size_t fromRowIndex = 0, size_t size = 0) const;

    private:
        size_t CorrectRangeSize(size_t fromRowIndex, size_t size) const;

        std::vector<DataSetExampleType> _examples;
        size_t _maxExampleSize = 0;
    };

    // friendly name
    typedef RowDataset<AutoSupervisedExample> AutoSupervisedDataset;

    /// <summary> Prints a row dataset to an ostream. </summary>
    ///
    /// <param name="os"> [in,out] The ostream to write data to. </param>
    /// <param name="dataset"> The dataset. </param>
    ///
    /// <returns> The ostream. </returns>
    template <typename DataSetExampleType>
    std::ostream& operator<<(std::ostream& os, const RowDataset<DataSetExampleType>& dataset);
}
}

#include "../tcc/RowDataset.tcc"
