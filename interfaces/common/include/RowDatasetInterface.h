////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDatasetInterface.h (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// dataset
#include "RowDataset.h"
#include "DenseDataVector.h"
#include "SupervisedExample.h"

// utilities
#include "StlIterator.h"
#include "AnyIterator.h"

// stl
#include <vector>
#include <random>

namespace dataset
{
    typedef GenericRowDataset::Iterator GenericRowIterator;
}

namespace interfaces
{
    class GenericRowDataset
    {
    public:
        GenericRowDataset() {};
        GenericRowDataset(const GenericRowDataset& other);
        GenericRowDataset(GenericRowDataset&& other) = default;
        const GenericRowDataset& operator=(GenericRowDataset&& other)
        {
            _dataset = std::move(other._dataset);
            return *this;
        }

        /// <summary> Constructor for RowDataset wrapper</summary>
        ///
        /// <param name="dataset"> The dataset::RowDataset to wrap </param>
        GenericRowDataset(dataset::GenericRowDataset&& dataset);

        /// <summary> Returns the number of examples in the dataset. </summary>
        ///
        /// <returns> The number of examples. </returns>
        uint64_t NumExamples() const;

        /// <summary> Returns the maximal size of any example. </summary>
        ///
        /// <returns> The maximal size of any example. </returns>
        uint64_t GetMaxDataVectorSize() const;

        /// <summary> Returns an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        dataset::GenericSupervisedExample GetExample(uint64_t index) const;
                
        /// <summary> Returns an example as a `DoubleDataVector`. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        dataset::SupervisedExample<dataset::DoubleDataVector> GetDenseSupervisedExample(uint64_t index) const;
                
        /// <summary> Returns an iterator that traverses the examples. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="numExamples"> The number of examples to iterate over, a value of zero means all
        /// examples. </param>
        ///
        /// <returns> The iterator. </returns>
        dataset::GenericRowIterator GetIterator(uint64_t firstExample = 0, uint64_t numExamples = 0) const;

        /// <summary> Adds an example at the bottom of the matrix. </summary>
        ///
        /// <param name="example"> [in,out] The example. </param>
        void AddExample(dataset::GenericSupervisedExample&& example);

        /// <summary> Randomly permutes the rows of the dataset. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        void RandomPermute(std::default_random_engine& rng);

        /// <summary>
        /// Permutes the dataset so that the first `count` examples are chosen uniformly, and the rest are arbitrary.
        /// </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="count"> Number of examples to permute. </param>
        void RandomPermute(std::default_random_engine& rng, uint64_t count);

    private:
        dataset::GenericRowDataset _dataset;
    };
}

