////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     DataVectorBuilder.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "DenseDataVector.h"
#include "OnesDataVector.h"
#include "ZeroDataVector.h"

// stl
#include <type_traits>

namespace dataset
{
    template<typename DefaultDataVectorType>
    template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept>
    std::unique_ptr<IDataVector> DataVectorBuilder<DefaultDataVectorType>::Build(IndexValueIteratorType IndexValueIterator)
    {
        static_assert(std::is_same<DefaultDataVectorType, FloatDataVector>::value || std::is_same<DefaultDataVectorType, SparseFloatDataVector>::value, "default DataVector type can be either FloatDataVector or SparseFloatDataVector");

        auto up_vec = std::make_unique<DefaultDataVectorType>();

        bool containsNonBinary = false;
        bool containsNonShorts = false;
        uint64_t numNonZeros = 0;

        while(IndexValueIterator.IsValid())
        {
            // add entry to vector
            auto entry = IndexValueIterator.Get();
            up_vec->AppendEntry(entry.index, entry.value);

            // Update VectorStats
            if(entry.value != 1.0)
            {
                containsNonBinary = true;
            }

            if(((short)entry.value) != entry.value)
            {
                containsNonShorts = true;
            }

            ++numNonZeros;

            // next
            IndexValueIterator.Next();
        }

        // Logic to choose the best representation
        
        // sparse
        if(numNonZeros * 3 < up_vec->Size())
        {
            // binary
            if(!containsNonBinary)
            {
                return std::make_unique<SparseBinaryDataVector>(up_vec->GetIterator());
            }

            // short sparse
            else if(!containsNonShorts)
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
            if(numNonZeros == 0)
            {
                return std::make_unique<ZeroDataVector>();
            }

            // all ones
            else if(!containsNonBinary && numNonZeros == up_vec->Size())
            {
                return std::make_unique<OnesDataVector>(up_vec->Size());
            }

            // other dense
            else
            {
                if(std::is_same<DefaultDataVectorType, FloatDataVector>::value)
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
