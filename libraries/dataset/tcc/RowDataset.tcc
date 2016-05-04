////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDataset.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <stdexcept>
#include <random>
#include <cassert>
#include <algorithm>

namespace dataset
{
    template<typename DataVectorType>
    uint64_t RowDataset<DataVectorType>::NumExamples() const
    {
        return _examples.size();
    }

    template<typename DataVectorType>
    uint64_t RowDataset<DataVectorType>::GetMaxDataVectorSize() const
    {
        return _maxExampleSize;
    }

    template<typename DataVectorType>
    const SupervisedExample<DataVectorType>& RowDataset<DataVectorType>::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    template<typename DataVectorType>
    const SupervisedExample<DataVectorType>& RowDataset<DataVectorType>::operator[](uint64_t index) const
    {
        return _examples[index];
    }

    template<typename DataVectorType>
    typename RowDataset<DataVectorType>::Iterator RowDataset<DataVectorType>::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        if (firstExample >= NumExamples())
        {
            return utilities::MakeStlIterator(_examples.cend(), _examples.cend());
        }

        uint64_t lastExample = firstExample + numExamples;
        if (lastExample > NumExamples() || numExamples == 0)
        {
            lastExample = NumExamples();
        }

        return utilities::MakeStlIterator(_examples.cbegin() + firstExample, _examples.cbegin() + lastExample);
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::AddExample(RowType example)
    {
        uint64_t size = example.GetDataVector().Size();
        _examples.push_back(std::move(example));

        if (_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::Reset()
    {
        _examples.clear();
        _maxExampleSize = 0;
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::RandPerm(std::default_random_engine& rng)
    {
        RandPerm(rng, NumExamples());
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::RandPerm(std::default_random_engine& rng, uint64_t count)
    {
        using std::swap;
        uint64_t lastIndex = NumExamples() - 1;

        for (uint64_t i = 0; i < count; ++i)
        {
            std::uniform_int_distribution<uint64_t> dist(i, lastIndex);
            uint64_t j = dist(rng);
            swap(_examples[i], _examples[j]);
        }
    }

    template<typename DataVectorType>
    template<typename SortKeyType>
    void RowDataset<DataVectorType>::Sort(SortKeyType sortKey, uint64_t fromRowIndex, uint64_t size)
    {

        assert(fromRowIndex + size <= _examples.size());

        if (size == 0)
        {
            size = _examples.size() - fromRowIndex;
        }
        if(size <= 1)
        {
            return;
        }

        std::sort(_examples.begin() + fromRowIndex, 
        _examples.begin() + fromRowIndex + size, 
        [&](const RowType& a, const RowType& b) -> bool 
        {
            return sortKey(a) < sortKey(b);
        });
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::Print(std::ostream& os) const
    {
        for(const auto& example : _examples)
        {
            os << example << std::endl;
        }
    }

    template<typename DataVectorType>
    std::ostream& operator<<(std::ostream& os, RowDataset<DataVectorType>& dataset)
    {
        dataset.Print(os);
        return os;
    }
}

