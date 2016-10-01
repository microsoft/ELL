////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury
//
////////////////////////////////////////////////////////////////////////////////////////////////////
%ignore emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>::GetPredictor() const;
%ignore emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>::GetPredictor() const;

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

%template () emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>;
%template () emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>;

class SGDTrainerProxy;
%nodefaultctor SGDTrainerProxy;

class SortingForestTrainerProxy;
%nodefaultctor SortingForestTrainerProxy;

#if defined(SWIGJAVASCRIPT)
#define EMLL_SWIGJAVASCRIPT 1

%{
	#define EMLL_SWIGJAVASCRIPT 1
	template <typename PredictorType, typename IteratorType>
	class TrainWorker : public Nan::AsyncWorker
	{
		public:	
		TrainWorker(Nan::Callback* doneCallback, std::shared_ptr<emll::trainers::IIncrementalTrainer<PredictorType>> trainer, emll::dataset::GenericRowDataset::Iterator exampleIterator) : Nan::AsyncWorker(doneCallback), _exampleIterator(exampleIterator)
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
		std::shared_ptr<emll::trainers::IIncrementalTrainer<PredictorType>> _trainer = nullptr;
		IteratorType _exampleIterator;
	};
%}
#endif

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

			#if defined(EMLL_SWIGJAVASCRIPT)
			void UpdateAsync(emll::dataset::GenericRowDataset::Iterator exampleIterator, Callback doneCb) {
				auto doneCallback = doneCb.GetFunction();
				Nan::AsyncQueueWorker(new TrainWorker<emll::predictors::LinearPredictor, emll::dataset::GenericRowDataset::Iterator>(doneCallback, _trainer, exampleIterator));
			}
			#endif

			const emll::predictors::LinearPredictor& GetPredictor() const {
				return *_trainer->GetPredictor();
			}

		private:
			std::shared_ptr<emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>> _trainer = nullptr;			
	};

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
%}

