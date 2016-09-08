////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelAsync.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Our interface classes
%{
#include "LoadModel.h"

#include <string>
#include <functional>
#include <iostream>
%}

%include "LoadModel.h"

%{
	class LoadModelWorker : public Nan::AsyncWorker
	{
	public:
		LoadModelWorker(Nan::Callback* doneCallback, std::string filename) : Nan::AsyncWorker(doneCallback), _filename(filename)
		{
		}

		virtual void Execute() override
		{
			// call load model here
			_model = common::LoadModel(_filename);
		}

		virtual void HandleOKCallback() override
		{
			Nan::HandleScope scope;
			// passing out a new pointer to JS world, why cant we send a copy??
			v8::Handle<v8::Value> jsresult = SWIG_NewPointerObj(SWIG_as_voidptr(&_model), SWIGTYPE_p_model__Model, 0 | 0);
			v8::Local<v8::Value> argv[] = { jsresult };
			callback->Call(1, argv);
		}

	private:
		std::string _filename;
		model::Model _model;
	};
%}

%inline {
    void LoadModelAsync(std::string filename, Callback doneCb)
    {
       auto doneCallback = doneCb.GetFunction();
       Nan::AsyncQueueWorker(new LoadModelWorker(doneCallback, filename));
    }
}

