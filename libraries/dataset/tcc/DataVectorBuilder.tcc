// DataVectorBulder.tcc

#include "SparseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "DenseDataVector.h"
#include "OnesDataVector.h"
#include "ZeroDataVector.h"

#include "types.h"


#include <memory>
using std::move;
using std::make_unique;

#include <type_traits>
using std::is_same;

namespace dataset
{
    template<typename DefaultDataVectorType>
    template<typename IndexValueIteratorType>
    unique_ptr<IDataVector> DataVectorBuilder<DefaultDataVectorType>::Build(IndexValueIteratorType indexValueIterator)
    {
        static_assert(is_same<DefaultDataVectorType, FloatDataVector>::value || is_same<DefaultDataVectorType, SparseFloatDataVector>::value, "default DataVector type can be either FloatDataVector or SparseFloatDataVector");

        auto up_vec = make_unique<DefaultDataVectorType>();

        bool containsNonBinary = false;
        bool containsNonShorts = false;
        uint64 numNonZeros = 0;

        while(indexValueIterator.IsValid())
        {
            // add entry to vector
            auto entry = indexValueIterator.Get();
            up_vec->PushBack(entry.index, entry.value);

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
            indexValueIterator.Next();
        }

        // Logic to choose the best representation
        
        // sparse
        if(numNonZeros * 3 < up_vec->Size())
        {
            // binary
            if(!containsNonBinary)
            {
                return make_unique<SparseBinaryDataVector>(up_vec->GetIterator());
            }

            // short sparse
            else if(!containsNonShorts)
            {
                return make_unique<SparseShortDataVector>(up_vec->GetIterator());
            }

            // other sparse
            else
            {
                if (is_same<DefaultDataVectorType, SparseFloatDataVector>::value)
                {
                    return move(up_vec);
                }
                else
                {
                    return make_unique<SparseFloatDataVector>(up_vec->GetIterator());
                }
            }
        }

        // dense
        else
        {
            // all zeros (this is considered a dense vector since its Size() is zero, so the fraction of nonZeros is 0/0)
            if(numNonZeros == 0)
            {
                return make_unique<ZeroDataVector>();
            }

            // all ones
            else if(numNonZeros == up_vec->Size())
            {
                return make_unique<OnesDataVector>(up_vec->Size());
            }

            // other dense
            else
            {
                if(is_same<DefaultDataVectorType, FloatDataVector>::value)
                {
                    return move(up_vec);
                }
                else
                {
                    return make_unique<FloatDataVector>(up_vec->GetIterator());
                }
            }
        }

        // this point should never be reached
    }
}