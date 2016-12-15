////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury
//
////////////////////////////////////////////////////////////////////////////////////////////////////
%ignore ell::trainers::IIncrementalTrainer<ell::predictors::LinearPredictor>::GetPredictor() const;
%ignore ell::trainers::IIncrementalTrainer<ell::predictors::SimpleForestPredictor>::GetPredictor() const;

%{
#define SWIG_FILE_WITH_INIT

#include "MultiEpochIncrementalTrainer.h"
#include "IIncrementalTrainer.h"
#include "SGDIncrementalTrainer.h"
#include "MakeTrainer.h"
%}

%include "SGDIncrementalTrainer.h"
%include "MultiEpochIncrementalTrainer.h"
%include "IIncrementalTrainer.h"

%template () ell::trainers::IIncrementalTrainer<ell::predictors::LinearPredictor>;
%template () ell::trainers::IIncrementalTrainer<ell::predictors::SimpleForestPredictor>;

class SGDTrainerProxy;
%nodefaultctor SGDTrainerProxy;

class SortingForestTrainerProxy;
%nodefaultctor SortingForestTrainerProxy;

#if defined(SWIGJAVASCRIPT)
#define ELL_SWIGJAVASCRIPT 1

%{
	#define ELL_SWIGJAVASCRIPT 1
	template <typename PredictorType, typename IteratorType>
	class TrainWorker : public Nan::AsyncWorker
	{
		public:	
		TrainWorker(Nan::Callback* doneCallback, std::shared_ptr<ell::trainers::IIncrementalTrainer<PredictorType>> trainer, ell::dataset::GenericRowDataset::Iterator exampleIterator) : Nan::AsyncWorker(doneCallback), _exampleIterator(exampleIterator)
		{
			_trainer = trainer; 
		}

		virtual void Execute() override
		{
			_trainer->Update(_exampleIterator);
		}

		virtual void HandleOKCallback() override
		{
			Nan::HandleScope scope;
			bool fResult = true; // just passing a bool for the result of training
			
			v8::Handle<v8::Value> jsresult = SWIGV8_BOOLEAN_NEW(fResult);
			v8::Local<v8::Value> argv[] = { jsresult };
			callback->Call(1, argv);
		}

	private:	
		std::shared_ptr<ell::trainers::IIncrementalTrainer<PredictorType>> _trainer = nullptr;
		IteratorType _exampleIterator;
	};
%}
#endif

%inline %{
	class SGDTrainerProxy
	{
		public:
			SGDTrainerProxy() {}
			SGDTrainerProxy(std::unique_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::LinearPredictor>>& trainer)
			{
				_trainer = std::shared_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::LinearPredictor>>(trainer.release());
			}

			void Update(ell::dataset::GenericRowDataset::Iterator exampleIterator) {
				_trainer->Update(exampleIterator);
			}

			#if defined(ELL_SWIGJAVASCRIPT)
			void UpdateAsync(ell::dataset::GenericRowDataset::Iterator exampleIterator, Callback doneCb) {
				auto doneCallback = doneCb.GetFunction();
				Nan::AsyncQueueWorker(new TrainWorker<ell::predictors::LinearPredictor, ell::dataset::GenericRowDataset::Iterator>(doneCallback, _trainer, exampleIterator));
			}
			#endif

			const ell::predictors::LinearPredictor& GetPredictor() const {
				return *_trainer->GetPredictor();
			}

		private:
			std::shared_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::LinearPredictor>> _trainer = nullptr;			
	};

	class SortingForestTrainerProxy
	{
		public:
			SortingForestTrainerProxy() {}
			SortingForestTrainerProxy(std::unique_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::SimpleForestPredictor>>& trainer)
			{
				_trainer = std::shared_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::SimpleForestPredictor>>(trainer.release());
			}

			void Update(ell::dataset::GenericRowDataset::Iterator exampleIterator) {
				_trainer->Update(exampleIterator);
			}

			const ell::predictors::SimpleForestPredictor& GetPredictor() const {
				return *_trainer->GetPredictor();
			}

		private:
			std::shared_ptr<ell::trainers::IIncrementalTrainer<ell::predictors::SimpleForestPredictor>> _trainer = nullptr;			
	};	
%}
