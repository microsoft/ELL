////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     common.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include "DataLoadArguments.h"
#include "MapLoadArguments.h"
#include "LoadModel.h"

#include "TrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "ForestTrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"

%}

%ignore ell::common::GetDataset;

%include "ModelLoadArguments.h"
%include "MapLoadArguments.h"
%include "LoadModel.h"

// Trainer arguments
%include "TrainerArguments.h"
%include "SGDIncrementalTrainerArguments.h"
%include "ForestTrainerArguments.h"
%include "MultiEpochIncrementalTrainerArguments.h"

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
