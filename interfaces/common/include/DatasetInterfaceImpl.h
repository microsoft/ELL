////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DatasetInterfaceImpl.h (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Dataset.h"
#include "DatasetInterface.h"
#include "AutoDataVector.h"

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

} // end namespace
