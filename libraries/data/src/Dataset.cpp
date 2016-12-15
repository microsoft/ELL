////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Dataset.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Dataset.h"

namespace ell
{
namespace data
{
    AnyDataset::AnyDataset(const DatasetBase* pDataset, size_t fromIndex, size_t size)
        : _pDataset(pDataset), _fromIndex(fromIndex), _size(size)
    {
    }
}
}