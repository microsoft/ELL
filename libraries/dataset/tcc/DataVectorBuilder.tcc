////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVectorBuilder.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"

// stl
#include <type_traits>

namespace emll
{
namespace dataset
{
    template <typename DefaultDataVectorType>
    template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept>
    std::unique_ptr<IDataVector> DataVectorBuilder<DefaultDataVectorType>::Build(IndexValueIteratorType IndexValueIterator)
    {
        static_assert(std::is_same<DefaultDataVectorType, FloatDataVector>::value || std::is_same<DefaultDataVectorType, SparseFloatDataVector>::value, "default DataVector type can be either FloatDataVector or SparseFloatDataVector");

        auto up_vec = std::make_unique<DefaultDataVectorType>();

        bool containsNonBinary = false;
        bool containsNonShorts = false;
        size_t numNonZeros = 0;

        while (IndexValueIterator.IsValid())
        {
            // add element to vector
            auto element = IndexValueIterator.Get();
            up_vec->AppendElement(element.index, element.value);

            // Update VectorStats
            if (element.value != 1.0)
            {
                containsNonBinary = true;
            }

            if (((short)element.value) != element.value)
            {
                containsNonShorts = true;
            }

            ++numNonZeros;

            // next
            IndexValueIterator.Next();
        }

        // Logic to choose the best representation

        // sparse
        if (numNonZeros * 3 < up_vec->Size())
        {
            // binary
            if (!containsNonBinary)
            {
                return std::make_unique<SparseBinaryDataVector>(up_vec->GetIterator());
            }

            // short sparse
            else if (!containsNonShorts)
            {
                return std::make_unique<SparseShortDataVector>(up_vec->GetIterator());
            }

            // other sparse
            else
            {
                if (std::is_same<DefaultDataVectorType, SparseFloatDataVector>::value)
                {
                    return std::move(up_vec);
                }
                else
                {
                    return std::make_unique<SparseFloatDataVector>(up_vec->GetIterator());
                }
            }
        }

        // dense
        else
        {
            // all zeros (this is considered a dense vector since its Size() is zero, so the fraction of nonZeros is 0/0)
            if (numNonZeros == 0)
            {
                return std::make_unique<FloatDataVector>();
            }

            // other dense
            else
            {
                if (std::is_same<DefaultDataVectorType, FloatDataVector>::value)
                {
                    return std::move(up_vec);
                }
                else
                {
                    return std::make_unique<FloatDataVector>(up_vec->GetIterator());
                }
            }
        }

        // this point should never be reached
    }
}
}
