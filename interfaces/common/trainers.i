////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
%ignore emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>::GetPredictor() const;
%ignore emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>::GetPredictor() const;


%{
#define SWIG_FILE_WITH_INIT
#include "ForestTrainerArguments.h"
#include "MultiEpochIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "TrainerArguments.h"
#include "IIncrementalTrainer.h"
#include "SGDIncrementalTrainer.h"
#include "MakeTrainer.h"

typedef emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor> IncrementalLinearPredictorTrainer;
typedef emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor> IncrementalForestPredictorTrainer;
%}

%include "SGDIncrementalTrainer.h"
%include "ForestTrainerArguments.h"
%include "MultiEpochIncrementalTrainer.h"
%include "MultiEpochIncrementalTrainerArguments.h"
%include "SGDIncrementalTrainerArguments.h"
%include "TrainerArguments.h"
%include "IIncrementalTrainer.h"

%template () emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>;
%template () emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>;

typedef emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor> IncrementalLinearPredictorTrainer;
typedef emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor> IncrementalForestPredictorTrainer;

%include "unique_ptr.i"
wrap_unique_ptr(LinearPredictorPtr, IncrementalLinearPredictorTrainer)
wrap_unique_ptr(SimpleForestPredictorPtr, IncrementalForestPredictorTrainer)

%include "noncopyable.i"

class SGDTrainerProxy;
%nodefaultctor SGDTrainerProxy;

class SortingForestTrainerProxy;
%nodefaultctor SortingForestTrainerProxy;

%inline %{
	class SGDTrainerProxy
	{
		public:
			SGDTrainerProxy() {}
			SGDTrainerProxy(std::unique_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>>& trainer)
			{
				_trainer = std::shared_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>>(trainer.release());
			}

			void Update(emll::dataset::GenericRowDataset::Iterator exampleIterator) {
				_trainer->Update(exampleIterator);
			}

			const emll::predictors::LinearPredictor& GetPredictor() const {
				return *_trainer->GetPredictor();
			}

		private:
			std::shared_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>> _trainer = nullptr;			
	};

	SGDTrainerProxy GetSGDIncrementalTrainer(uint64_t dim, const emll::common::LossArguments& lossArguments, const emll::common::SGDIncrementalTrainerArguments& trainerArguments)
	{
		return SGDTrainerProxy(emll::common::MakeSGDIncrementalTrainer(dim, lossArguments, trainerArguments));
	}

	class SortingForestTrainerProxy
	{
		public:
			SortingForestTrainerProxy() {}
			SortingForestTrainerProxy(std::unique_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>>& trainer)
			{
				_trainer = std::shared_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>>(trainer.release());
			}

			void Update(emll::dataset::GenericRowDataset::Iterator exampleIterator) {
				_trainer->Update(exampleIterator);
			}

			const emll::predictors::SimpleForestPredictor& GetPredictor() const {
				return *_trainer->GetPredictor();
			}

		private:
			std::shared_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>> _trainer = nullptr;			
	};

	SortingForestTrainerProxy GetForestTrainer(const emll::common::LossArguments& lossArguments, const emll::common::ForestTrainerArguments& trainerArguments)
	{
		return SortingForestTrainerProxy(emll::common::MakeSortingForestTrainer(lossArguments, trainerArguments));
	}
%}

