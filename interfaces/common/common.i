////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     common.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include "DataLoadArguments.h"
#include "DataLoadersInterface.h"
#include "LoadModel.h"

#include "TrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "ForestTrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"

%}

%ignore emll::common::GetDataset;

%include "LoadModel.h"

// Modified versions of interfaces
%include "DataLoadersInterface.h"

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
	SGDTrainerProxy GetSGDIncrementalTrainer(uint64_t dim, const emll::common::LossArguments& lossArguments, const emll::common::SGDIncrementalTrainerArguments& trainerArguments)
	{
		auto trainer = emll::common::MakeSGDIncrementalTrainer(dim, lossArguments, trainerArguments);
		return SGDTrainerProxy(trainer);
	}

	SortingForestTrainerProxy GetForestTrainer(const emll::common::LossArguments& lossArguments, const emll::common::ForestTrainerArguments& trainerArguments)
	{
		auto trainer = emll::common::MakeSortingForestTrainer(lossArguments, trainerArguments);
		return SortingForestTrainerProxy(trainer);
	}
%}
