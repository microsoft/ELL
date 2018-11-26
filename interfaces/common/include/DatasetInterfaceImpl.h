////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DatasetInterfaceImpl.h (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DatasetInterface.h"

#include <data/include/AutoDataVector.h>
#include <data/include/Dataset.h>

namespace ELL_API
{
class AutoDataVector::AutoDataVectorImpl
{
public:
    std::shared_ptr<const ell::data::AutoDataVector> _vector;
};

class AutoSupervisedDataset::AutoSupervisedDatasetImpl
{
public:
    ell::data::AutoSupervisedDataset _dataset;
};

} // namespace ELL_API
