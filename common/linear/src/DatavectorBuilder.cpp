// DatavectorBuilder.cpp

#include "DatavectorBuilder.h"
#include "DenseDatavector.h"
#include "SparseDatavector.h"
#include "SparseBinaryDatavector.h"
#include "ZeroDatavector.h"
#include "OnesDatavector.h"

using std::move;
using std::make_unique;

namespace linear
{
    DatavectorBuilder::DatavectorBuilder() 
    {
        _up_vec = make_unique<SparseFloatDatavector>();
    }

    void DatavectorBuilder::PushBack(uint index, double value)
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

    unique_ptr<IDataVector> DatavectorBuilder::GetVectorAndReset()
    {
        unique_ptr<IDataVector> output_vec = nullptr;
        
        // sparse
        if(_vector_stats.numNonzeros * 3 < _up_vec->Size())
        {
            ++_builder_stats.num_sparse;

            // binary
            if(!_vector_stats.contains_nonbinary)
            {
                output_vec = make_unique<SparseBinaryDatavector>(*_up_vec);
            }

            // short sparse
            else if(!_vector_stats.contains_non_shorts)
            {
                output_vec = make_unique<SparseShortDatavector>(*_up_vec);
            }

            // other sparse
            else
            {
                output_vec = make_unique<SparseFloatDatavector>(*_up_vec);
            }
        }

        // dense
        else
        {
            ++_builder_stats.num_dense;

            // all zeros
            if(_vector_stats.numNonzeros == 0)
            {
                output_vec = make_unique<ZeroDatavector>(*_up_vec);
            }

            // all ones
            else if(_vector_stats.numNonzeros == _up_vec->Size())
            {
                output_vec = make_unique<OnesDatavector>(*_up_vec);
            }

            // other dense
            else
            {
                output_vec = make_unique<FloatDatavector>(*_up_vec);
            }
        }
        
        // Reset
        _up_vec->Reset();
        _vector_stats = VectorStats();
        
        // return
        return move(output_vec);
    }
}