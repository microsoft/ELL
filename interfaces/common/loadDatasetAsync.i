////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     loadDatasetAsync.i (interfaces)
//  Authors:  Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include <string>
#include <functional>
#include <iostream>
#include "RowDataset.h"
%}

%{
	class GetDatasetWorker : public Nan::AsyncWorker
	{
	public:
		GetDatasetWorker(Nan::Callback* doneCallback, std::string filename) : Nan::AsyncWorker(doneCallback), _filename(filename)
		{
		}

		virtual void Execute() override
		{
			// call load dataset here
			_dataset = emll::interfaces::GetDataset(_filename);
		}

		virtual void HandleOKCallback() override
		{
			Nan::HandleScope scope;
			// passing out a new pointer to JS world, why cant we send a copy??
			v8::Handle<v8::Value> jsresult = SWIG_NewPointerObj(SWIG_as_voidptr(&_dataset), SWIGTYPE_p_emll__dataset__RowDatasetT_emll__dataset__ExampleT_emll__dataset__IDataVector_emll__dataset__WeightLabel_t_t, 0 | 0);
			v8::Local<v8::Value> argv[] = { jsresult };
			callback->Call(1, argv);
		}

	private:
		std::string _filename;
		emll::dataset::GenericRowDataset _dataset;
	};	
%}

%inline {
    void GetDatasetAsync(std::string dataFilename, Callback doneCb)
    {
       auto doneCallback = doneCb.GetFunction();
       Nan::AsyncQueueWorker(new GetDatasetWorker(doneCallback, dataFilename));
    }	
}
