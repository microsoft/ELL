// DataVectorBuilder.cpp

#include "DataVectorBuilder.h"
#include "DenseDataVector.h"
#include "SparseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "ZeroDataVector.h"
#include "OnesDataVector.h"

using std::move;
using std::make_unique;

namespace linear
{
    DataVectorBuilder::DataVectorBuilder() 
    {
        _up_vec = make_unique<SparseFloatDataVector>();
    }

    void DataVectorBuilder::PushBack(uint index, double value)
    {

        if(value == 0)
        {
            return;
        }

        _up_vec->PushBack(index, value);
        
        // Update VectorStats
        if(value != 1)
        {
            _vector_stats.contains_nonbinary = true;
        }

        if(((short)value) != value)
        {
            _vector_stats.contains_non_shorts = true;
        }

        ++_vector_stats.numNonzeros;
    }

    unique_ptr<IDataVector> DataVectorBuilder::GetVectorAndReset()
    {
        unique_ptr<IDataVector> output_vec = nullptr;
        
        // sparse
        if(_vector_stats.numNonzeros * 3 < _up_vec->Size())
        {
            ++_builder_stats.num_sparse;

            // binary
            if(!_vector_stats.contains_nonbinary)
            {
                output_vec = make_unique<SparseBinaryDataVector>(*_up_vec);
            }

            // short sparse
            else if(!_vector_stats.contains_non_shorts)
            {
                output_vec = make_unique<SparseShortDataVector>(*_up_vec);
            }

            // other sparse
            else
            {
                output_vec = make_unique<SparseFloatDataVector>(*_up_vec);
            }
        }

        // dense
        else
        {
            ++_builder_stats.num_dense;

            // all zeros
            if(_vector_stats.numNonzeros == 0)
            {
                output_vec = make_unique<ZeroDataVector>(*_up_vec);
            }

            // all ones
            else if(_vector_stats.numNonzeros == _up_vec->Size())
            {
                output_vec = make_unique<OnesDataVector>(*_up_vec);
            }

            // other dense
            else
            {
                output_vec = make_unique<FloatDataVector>(*_up_vec);
            }
        }
        
        // Reset
        _up_vec->Reset();
        _vector_stats = VectorStats();
        
        // return
        return move(output_vec);
    }
}