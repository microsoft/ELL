////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     RowDataset.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RowDataset.h"
#include "SupervisedExample.h"

#include "types.h"

// utilities
#include "StlIterator.h"

// stl
#include <vector>
#include <random>

namespace dataset
{
    class RowDataset 
    {
    public:
        using Iterator = utilities::StlIterator<std::vector<SupervisedExample>::const_iterator>;

        /// <summary> Default constructor. </summary>
        RowDataset() = default;

        /// <summary> Move constructor </summary>
        RowDataset(RowDataset&&) = default;

        /// <summary> Deleted copy constructor. </summary>
        RowDataset(const RowDataset&) = delete;

        /// <summary> Default move assignment operator. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        RowDataset& operator=(RowDataset&&) = default;

        /// <summary> Deleted asignment operator. </summary>
        RowDataset& operator=(const RowDataset&) = delete;

        /// <summary> Returns the number of examples in the dataset. </summary>
        ///
        /// <returns> The number of examples. </returns>
        uint64 NumExamples() const;

        /// <summary> Returns the maximal size of any example. </summary>
        ///
        /// <returns> The maximal size of any example. </returns>
        uint64 GetMaxExampleSize() const;

        /// <summary> Returns a reference to an example. </summary>
        ///
        /// <param name="index"> Zero-based index of the row. </param>
        ///
        /// <returns> Reference to the specified example. </returns>
        const SupervisedExample& GetExample(uint64 index) const;

        /// <summary> Returns an iterator that traverses the examples. </summary>
        ///
        /// <param name="firstExample"> Zero-based index of the first example to iterate over. </param>
        /// <param name="numExamples"> The number of examples to iterate over, a value of zero means all
        /// examples. </param>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator(uint64 firstExample = 0, uint64 numExamples = 0) const;

        /// <summary> Adds an example at the bottom of the matrix. </summary>
        ///
        /// <param name="example"> [in,out] The example. </param>
        void AddExample(SupervisedExample&& example);

        /// <summary> Randomly permutes the rows of the dataset. </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        void RandPerm(std::default_random_engine& rng);

        /// <summary>
        /// Permutes the dataset so that the first count examples are chosen uniformly, and the rest are arbitrary.
        /// </summary>
        ///
        /// <param name="rng"> [in,out] The random number generator. </param>
        /// <param name="count"> Number of examples to permute. </param>
        void RandPerm(std::default_random_engine& rng, uint64 count);

    private:
        std::vector<SupervisedExample> _examples;
        uint64 _maxExampleSize = 0;
    };
}
