////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     common.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include <common/include/DataLoadArguments.h>
#include <common/include/MapLoadArguments.h>
#include <common/include/LoadModel.h>
#include <common/include/TrainerArguments.h>
#include <common/include/SGDIncrementalTrainerArguments.h>
#include <common/include/ForestTrainerArguments.h>
#include <common/include/MultiEpochIncrementalTrainerArguments.h>
%}

%ignore ell::common::GetDataset;

%include <common/include/ModelLoadArguments.h>
%include <common/include/MapLoadArguments.h>
%include <common/include/LoadModel.h>

// Trainer arguments
%include <common/include/TrainerArguments.h>
%include <common/include/SGDIncrementalTrainerArguments.h>
%include <common/include/ForestTrainerArguments.h>
%include <common/include/MultiEpochIncrementalTrainerArguments.h>

#if defined(SWIGJAVASCRIPT)
%include "loadModelAsync.i"
%include "loadDatasetAsync.i"
#endif

%inline %{
    SGDTrainerProxy GetSGDIncrementalTrainer(size_t dim, const ell::common::LossArguments& lossArguments, const ell::common::SGDIncrementalTrainerArguments& trainerArguments)
    {
        auto trainer = ell::common::MakeSGDIncrementalTrainer(dim, lossArguments, trainerArguments);
        return SGDTrainerProxy(trainer);
    }

    SortingForestTrainerProxy GetForestTrainer(const ell::common::LossArguments& lossArguments, const ell::common::ForestTrainerArguments& trainerArguments)
    {
        auto trainer = ell::common::MakeSortingForestTrainer(lossArguments, trainerArguments);
        return SortingForestTrainerProxy(trainer);
    }
%}
