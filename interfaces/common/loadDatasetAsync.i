////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     loadDatasetAsync.i (interfaces)
//  Authors:  Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <string>
#include <functional>
#include <iostream>
#include "RowDatasetInterface.h"
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
			_dataset = interfaces::GetDataset(_filename);
		}

		virtual void HandleOKCallback() override
		{
			Nan::HandleScope scope;
			// passing out a new pointer to JS world, why cant we send a copy??
			v8::Handle<v8::Value> jsresult = SWIG_NewPointerObj(SWIG_as_voidptr(&_dataset), SWIGTYPE_p_interfaces__GenericRowDataset, 0 | 0);
			v8::Local<v8::Value> argv[] = { jsresult };
			callback->Call(1, argv);
		}

	private:
		std::string _filename;
		interfaces::GenericRowDataset _dataset;
	};

	class GetDataIteratorWorker : public Nan::AsyncWorker
	{
	public:
		GetDataIteratorWorker(Nan::Callback* doneCallback, std::string filename) : Nan::AsyncWorker(doneCallback), _filename(filename)
		{
		}

		virtual void Execute() override
		{
			// Invoke GetDataIterator syncronously
			_dataiterator = interfaces::GetDataIterator(_filename);
			
		}

		virtual void HandleOKCallback() override
		{
			Nan::HandleScope scope;
			// passing out a new pointer to JS world, why cant we send a copy??
			v8::Handle<v8::Value> jsresult = SWIG_NewPointerObj(SWIG_as_voidptr(&_dataiterator), SWIGTYPE_p_utilities__AnyIteratorT_dataset__SupervisedExampleT_dataset__IDataVector_t_t, 0 | 0);
			
			v8::Local<v8::Value> argv[] = { jsresult };
			callback->Call(1, argv);
		}

	private:
		std::string _filename;
		utilities::AnyIterator<dataset::GenericSupervisedExample> _dataiterator;		
	};
%}

%inline {
    void GetDatasetAsync(std::string dataFilename, Callback doneCb)
    {
       auto doneCallback = doneCb.GetFunction();
       Nan::AsyncQueueWorker(new GetDatasetWorker(doneCallback, dataFilename));
    }

	void GetDataIteratorAsync(std::string dataFilename, Callback doneCb)
    {
       auto doneCallback = doneCb.GetFunction();
       Nan::AsyncQueueWorker(new GetDataIteratorWorker(doneCallback, dataFilename));
    }
}