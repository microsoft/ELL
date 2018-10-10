////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndexedContainer.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename VectorElementType, typename IndexedContainerElementType>
    IndexedContainerElementType VectorIndexedContainer<VectorElementType, IndexedContainerElementType>::Get(size_t index) const  
    {
        return IndexedContainerElementType((*this)[index]); 
    }
}
}
}
